#include "HttpClient.h"
#include "util/StringUtil.h"
#include <sstream>
#include <utility>
#include <pico/cyw43_arch.h>
#include <util/Logger.h>
#include <mbedtls/error.h>

//==============================RequestDataBuilder================================================
RequestDataBuilder::RequestDataBuilder() : RequestDataBuilder(HttpVersion::HTTP1) {

}

RequestDataBuilder::RequestDataBuilder(const HttpVersion version) {
    httpVersion = version;
    flush();
}


RequestDataBuilder& RequestDataBuilder::body(const std::string &body) {
    _body = body;
    return *this;
}

RequestDataBuilder& RequestDataBuilder::method(RequestMethod method) {
    _method = method;
    return *this;
}

RequestDataBuilder &RequestDataBuilder::url(const std::string &url) {
    _url = url;
    return *this;
}

RequestDataBuilder &RequestDataBuilder::header(const std::string &key, const std::string &value) {
    headers[key] = value;
    return *this;
}


// 构建
std::string RequestDataBuilder::build() {
    std::string retString;
    // 请求行
    parseRequestLine(retString);
    // 请求头
    parseRequestHeader(retString);
    // 请求体
    parseRequestBody(retString);

    // 刷新
    flush();

    Logger::debug("RequestDataBuilder", "构建的请求: " + retString);
    return retString;
}

// 构建请求行
void RequestDataBuilder::parseRequestLine(std::string &str) const {
    // POST /api/login HTTP/1.1\r\n
    if (_method == RequestMethod::DELETE) {
        str.append("DELETE");
    } else if (_method == RequestMethod::POST) {
        str.append("POST");
    } else if (_method == RequestMethod::PUT) {
        str.append("PUT");
    } else {
        str.append("GET");
    }

    str.append(" ");
    str.append(_url);

    if (httpVersion == HttpVersion::HTTP1_1) {
        str.append(" HTTP/1.1\r\n");
    } else if (httpVersion == HttpVersion::HTTP1) {
        str.append(" HTTP/1.0\r\n");
    }
}

// 构建请求头
void RequestDataBuilder::parseRequestHeader(std::string &str) {
    // 自动计算请求体长度
    if (!_body.empty()) headers["Content-Length"] = std::to_string(_body.size());

    // 遍历即可
    for (const auto& [key, value] : headers) {
        str.append(key).append(": ");
        str.append(value).append("\r\n");
    }

    str.append("\r\n");
}

// 构建请求体
void RequestDataBuilder::parseRequestBody(std::string &str) const {
    str.append(_body);
}


// 还原构造器状态为最初值
void RequestDataBuilder::flush() {
    _url.clear();
    _body.clear();
    _method = RequestMethod::GET;

    // 设置一些默认的请求头
    headers.clear();
    headers["User-Agent"] = "Pico2W";

    if (httpVersion == HttpVersion::HTTP1) {
        headers["Connection"] = "close";
    }
    if (httpVersion == HttpVersion::HTTP1_1) {
        headers["Connection"] = "Keep-Alive";
    }
}

//=================================Responser=========================================
Responser::Responser() = default;

// 解析请求头
size_t Responser::parseHeader(const std::string& data) {
    // 首先根据\r\n\r\n分割
    const size_t pos = data.find("\r\n\r\n");
    if (pos == std::string::npos) return std::string::npos;

    // 根据/r分
    std::istringstream iss(data.substr(0, pos));
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // 根据:分开保存
        const size_t headerPos = line.find(':');
        if (headerPos != std::string::npos){
            std::string key = line.substr(0, headerPos);
            std::string value = line.substr(headerPos + 1);
            StringUtil::trim(value);

            _headers[key] = value;
        }
    }

    return pos + 4;
}

// 解析响应体
void Responser::parseBody(const std::string& data) {
    // 去掉响应头
    const size_t pos = data.find("\r\n\r\n");
    if (pos == std::string::npos) return;

    _body = data.substr(pos + 4);
}

// 全部解析
void Responser::parseAll(const std::string &result) {
    parseHeader(result);
    parseBody(result);
}

// 获取响应头
std::string Responser::getHeader(const std::string &key) {
    return _headers[key];
}

// 获取响应体
std::string Responser::getBody() {
    return _body;
}

//=====================================HttpResponserParser====================================
HttpResponserParser::HttpResponserParser() = default;

bool HttpResponserParser::checkHttpOver() {
    // 看看当前数据里是否有\r\n
    const size_t pos = responserBuffer.find("\r\n\r\n");
    // 然后根据状态不同干不同的事情
    if (state == SENDING_HEADERS) {
        if (pos != std::string::npos) {
            // 有了 把状态修改并且寻找记录下响应体长度
            responser.parseHeader(responserBuffer);
            contentLength = atoi(responser.getHeader("Content-Length").c_str());
            state = SENDING_BODY;
        }
    }

    if (state == SENDING_BODY) {
        // 看看是不是到头了
        if (responserBuffer.size() - pos - 4 >= contentLength) {
            // 解析请求体 改状态 清空
            responser.parseBody(responserBuffer);
            state = PARSE_OVER;
        }
    }

    return state == PARSE_OVER;
}

Responser HttpResponserParser::getResponser() {
    // 看看是不是需要重置
    Responser temp = responser;
    if (this->state == PARSE_OVER) {
        this->responser = Responser();
        this->state = SENDING_HEADERS;
        this->responserBuffer.clear();
    }

    return temp;
}


void HttpResponserParser::requestSendOver() {
    state = SENDING_HEADERS;
}

void HttpResponserParser::appendResponserBuffer(const std::string &buffer) {
    responserBuffer += buffer;
}

// ==============================HttpsClient================================
HttpsClient::HttpsClient(const std::string &host, const int port, const char* ca)
    : _host(host), _port(port), pcb(tcp_new()) {
    // 各种初始化
    int ret = 0;
    Logger::debug("HttpsClient", "mbedtls开始初始化");
    state = HttpClientState::TLS_INIT;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&config);
    mbedtls_x509_crt_init(&cacert);

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctrDrbg);
    Logger::debug("HttpsClient", "mbedtls初始化完毕");

    // 初始化随机数生成器
    mbedtls_ctr_drbg_seed(&ctrDrbg, mbedtls_entropy_func, &entropy, nullptr, 0);
    Logger::debug("HttpsClient", "mbedtls随机数生成器初始化完毕");

    // 初始化SSL配置
    mbedtls_ssl_config_defaults(&config, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);

    // 绑定随机数生成器
    mbedtls_ssl_conf_rng(&config, mbedtls_ctr_drbg_random, &ctrDrbg);

    // 加载ca证书
    ret = mbedtls_x509_crt_parse(&cacert, reinterpret_cast<const unsigned char *>(ca), strlen(ca) + 1);
    if (ret != ERR_OK) {
        Logger::error("HttpsClient", "CA证书解析失败: " + std::to_string(ret));
        state = HttpClientState::ERROR;
        return;
    }
    Logger::debug("HttpsClient", "mbedtls CA证书加载完毕");

    // 配置ca信任链
    mbedtls_ssl_conf_ca_chain(&config, &cacert, nullptr);
    Logger::debug("HttpsClient", "mbedtls CA证书配置完毕");

    mbedtls_ssl_conf_authmode(&config, MBEDTLS_SSL_VERIFY_REQUIRED);

    // 创建SSL对象
    mbedtls_ssl_setup(&ssl, &config);
    Logger::debug("HttpsClient", "mbedtls SSL对象创建完毕");

    // 设置主机名
    mbedtls_ssl_set_hostname(&ssl, host.c_str());
    Logger::debug("HttpsClient", "mbedtls 设置目标主机名为: " + host);

    // 设置TCP
    mbedtls_ssl_set_bio(&ssl, this, sendTLS, recv, nullptr);
    Logger::debug("HttpsClient", "mbedtls 绑定TCP完毕");

    state = HttpClientState::TLS_INITED;
    responserParser = std::make_unique<HttpResponserParser>();

    state = HttpClientState::TCP_INIT;
    ip4addr_aton(_host.c_str(), &_addr);
    tcp_arg(pcb, this);
    err_t connectErr = tcp_connect(pcb, &_addr, _port, connectCallBack);
    if (connectErr != ERR_OK) {
        Logger::error("HttpsClient", "tcp_connect错误: " + std::to_string(connectErr));
        state = HttpClientState::ERROR;
        return;
    }

    tcp_recv(pcb, httpsRespCallBack);
    tcp_err(pcb, httpsErrorCallBack);
    Logger::debug("HttpsClient", "tcp绑定完毕");
    state = HttpClientState::TLS_INITED;

    tlsHand();
}

HttpsClient::~HttpsClient() {
    // 1. 尝试发送 TLS close_notify，通知对端正常关闭
    int ret = mbedtls_ssl_close_notify(&ssl);
    Logger::info("HttpsClient", "析构关闭TLS: " + std::to_string(ret));

    // 2. 关闭底层 TCP socket
    err_t err = tcp_close(pcb);
    Logger::info("HttpsClient", "析构关闭TCP: " + std::to_string(err));

    // 3. 释放 TLS 相关资源
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&config);

    // 4. 释放证书、随机数生成器等资源
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    Logger::info("HttpsClient", "Client TCP/TLS关闭");
}


// 连接完毕回调
err_t HttpsClient::connectCallBack(void *arg, tcp_pcb *pcb, err_t err) {
    // 如果不是成功
    auto* _this = static_cast<HttpsClient*>(arg);
    if (err != ERR_OK) {
        Logger::error("HttpsClient", "TCP连接错误: " + std::to_string(err));
        _this->state = HttpClientState::ERROR;
        return err;
    }

    // 改状态
    _this->state = HttpClientState::TCP_HANDSHAKE_OVER;
    return err;
}

// 致命错误回调
void HttpsClient::httpsErrorCallBack(void *arg, const err_t err) {
    auto* _this = static_cast<HttpsClient*>(arg);
    Logger::error("HttpsClient", "致命错误: " + std::to_string(err));
    _this->state = HttpClientState::ERROR;
}

int HttpsClient::sendTLS(void *ctx, const unsigned char *buf, size_t len) {
    auto* client = static_cast<HttpsClient*>(ctx);

    tcp_write(client->pcb, buf, len, TCP_WRITE_FLAG_COPY);
    tcp_output(client->pcb);

    return static_cast<int>(len);
}

int HttpsClient::recv(void *ctx, unsigned char *buf, size_t len) {
    auto* client = static_cast<HttpsClient*>(ctx);

    // 如果还没有数据
    if (client->unDecryResponser.empty()) return MBEDTLS_ERR_SSL_WANT_READ;

    size_t n = std::min(len, client->unDecryResponser.size());
    memcpy(buf, client->unDecryResponser.c_str(), n);
    client->unDecryResponser.erase(client->unDecryResponser.begin(), client->unDecryResponser.begin() + n);

    return static_cast<int>(n);
}

void HttpsClient::tlsHand() {
    // 首先等待握手完毕
    state = HttpClientState::TCP_HANDSHAKE;

    int usedMs = 0;
    while (state != HttpClientState::TCP_HANDSHAKE_OVER) {
        cyw43_arch_poll();
        sleep_ms(10);
        usedMs += 10;

        if (usedMs > 1000 * 5) {
            state = HttpClientState::ERROR;
            return;
        }
    }
    Logger::debug("HttpsClient", "TCP连接完毕");

    // 等待tls握手
    state = HttpClientState::TLS_HANDSHAKE;
    usedMs = 0;
    while (usedMs < 1000 * 5) {
        int ret = mbedtls_ssl_handshake(&ssl);
        if (ret == 0) {
            Logger::debug("HttpsClient", "TCP握手成功");
            state = HttpClientState::WAIT_REQUEST;
            return;
        }

        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            char errBuf[128]{};
            mbedtls_strerror(ret, errBuf, sizeof(errBuf));

            Logger::error("HttpsClient", "TLS握手失败，错误码: " + std::to_string(ret) +
                "，错误信息: " + std::string(errBuf));
            state = HttpClientState::ERROR;
            return;
        }

        cyw43_arch_poll();
        sleep_ms(10);
        usedMs += 10;
    }

    state = HttpClientState::ERROR;
}

err_t HttpsClient::httpsRespCallBack(void *arg, tcp_pcb *pb, pbuf *p, err_t err) {
    // 先把对象拿出来
    auto* _this = static_cast<HttpsClient*>(arg);

    // 处理特殊情况
    if (p == nullptr) {
        _this->state = HttpClientState::TCP_CLOSED;
        return ERR_OK;
    }

    // 把数据整理成一个str
    std::string data;
    data.resize(p->tot_len);
    pbuf_copy_partial(p, data.data(), p->tot_len, 0);

    // 存到未解码的后面
    _this->unDecryResponser += data;

    const u16_t receivedLength = p->tot_len;
    pbuf_free(p);
    tcp_recved(_this->pcb, receivedLength);

    return ERR_OK;
}

Responser HttpsClient::send(const std::string& request) {
    // 发送
    state = HttpClientState::REQUESTING;
    const char* data = request.c_str();
    mbedtls_ssl_write(&ssl, reinterpret_cast<const unsigned char*>(data), strlen(data));

    // 等待响应
    unsigned char buf[1024];
    state = HttpClientState::WAIT_RESPONSER;
    while (true) {
        cyw43_arch_poll();
        const int ref = mbedtls_ssl_read(&ssl, buf, sizeof(buf));

        // 是否已经完毕
        if (responserParser->checkHttpOver()) {
            state = HttpClientState::RESPONSER_OVER;
            break;
        }

        // 0: 对端关闭
        if (ref == 0) {
            state = HttpClientState::TCP_CLOSED;
            break;
        }

        // >0: 发来数据
        if (ref > 0) {
            std::string newData(reinterpret_cast<const char*>(buf), ref);
            this->responserParser->appendResponserBuffer(newData);
            continue;
        }

        // 继续等待
        if (ref == MBEDTLS_ERR_SSL_WANT_READ || ref == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }

        // 其他情况 错误
        Logger::error("HttpsClient", "TLS读取失败: " + std::to_string(ref));
        state = HttpClientState::ERROR;
        break;
    }

    return responserParser->getResponser();
}

// 是否初始化正确
bool HttpsClient::checkInitSuccess() const {
    // 检查下状态是不是ERROR
    return state != HttpClientState::ERROR;
}

// 响应是否正确
bool HttpsClient::checkResponserIsOk() const {
    // 检查下结果是不是 RESPONSER_OVER
    return state == HttpClientState::RESPONSER_OVER;
}

// ==============================HttpClient================================
HttpClient::HttpClient(std::string ip, int port): _ip(std::move(ip)), _port(port), pcb(tcp_new()) {

    responserParser = std::make_unique<HttpResponserParser>();

    ip4addr_aton(_ip.c_str(), &_addr);
    tcp_arg(pcb, this);
    tcp_connect(pcb, &_addr, _port, connectCallBack);
    tcp_recv(pcb, httpRespCallBack);
}

// 连接完毕回调
err_t HttpClient::connectCallBack(void *arg, tcp_pcb *pcb, err_t err) {
    // 改状态
    auto* _this = static_cast<HttpClient*>(arg);
    _this->connected = true;

    return ERR_OK;
}

// 发送数据
Responser HttpClient::send(const std::string &request) {
    // 首先等待连接完毕
    while (!this->connected) {
        cyw43_arch_poll();
        sleep_ms(1);
    }

    // 发送
    const char *data = request.c_str();
    tcp_write(pcb, data, strlen(data), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);

    // 请求处理完毕
    this->responserParser->requestSendOver();

    // 等待响应完毕
    while (!this->responserParser->checkHttpOver()) {
        cyw43_arch_poll();
        sleep_ms(1);
    }

    return this->responserParser->getResponser();
}

// 请求回调
err_t HttpClient::httpRespCallBack(void *arg, tcp_pcb *pb, pbuf *p, err_t err) {
    // 先把对象拿出来
    auto* _this = static_cast<HttpClient*>(arg);
    // 把数据整理成一个str
    std::string data;
    data.resize(p->tot_len);
    pbuf_copy_partial(p, data.data(), p->tot_len, 0);
    _this->responserParser->appendResponserBuffer(data);

    return ERR_OK;
}
