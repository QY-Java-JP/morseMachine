#ifndef MYPROJECT_HTTPCLIENT_H
#define MYPROJECT_HTTPCLIENT_H

#include <map>
#include <string>
#include "lwip/tcp.h"
#include <memory>
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

// 请求方法
enum class RequestMethod : uint8_t {
    GET = 1,
    PUT = 2,
    POST = 3,
    DELETE = 4
};

// HttpClient状态
enum class HttpClientState : uint8_t {
    // 错误
    ERROR = 0,
    // 未初始化
    UN_INIT = 1,
    // 正在初始化TLS
    TLS_INIT = 2,
    // TLS初始化完毕
    TLS_INITED = 3,
    // 正在初始化 TCP
    TCP_INIT = 4,
    // 初始化TCP完毕
    TCP_INITED = 5,
    // TCP握手中
    TCP_HANDSHAKE = 6,
    // TCP握手完毕
    TCP_HANDSHAKE_OVER = 7,
    // TLS握手中
    TLS_HANDSHAKE = 8,
    // TLS握手完毕
    TLS_HANDSHAKE_OVER = 9,

    // 等待发送数据
    WAIT_REQUEST = 10,
    // 发送数据中
    REQUESTING = 11,
    // 等待响应中
    WAIT_RESPONSER = 12,
    // 等待响应完毕
    RESPONSER_OVER = 13,

    // TCP管道被对方关闭
    TCP_CLOSED = 50
};

// http协议版本
enum class HttpVersion : uint8_t {
    // 1.0
    HTTP1 = 1,
    // 1.1
    HTTP1_1 = 2
};

// 请求报文建造者
/*
    POST /api/login HTTP/1.1\r\n
    Host: example.com\r\n
    Content-Type: application/json\r\n
    Content-Length: 39\r\n
    User-Agent: Pico2W\r\n
    Connection: close\r\n
    \r\n
    {"username":"ryu","password":"123"}
 */
class RequestDataBuilder {
    // 请求路径
    std::string _url;
    // 请求方法
    RequestMethod _method = RequestMethod::GET;
    // 请求体
    std::string _body;
    // 使用的http协议
    HttpVersion httpVersion = HttpVersion::HTTP1;

    // 所有请求头
    std::map<std::string, std::string> headers;

    // 清空所有设置 还原初始化
    void flush();

    // 构建请求行
    void parseRequestLine(std::string& str) const;

    // 构建请求头
    void parseRequestHeader(std::string& str);

    // 构建请求体
    void parseRequestBody(std::string& str) const;

public:
    RequestDataBuilder();

    explicit RequestDataBuilder(HttpVersion version);

    // 设置请求路径
    RequestDataBuilder& url(const std::string& url);

    // 设置请求方法
    RequestDataBuilder& method(RequestMethod method);

    // 设置请求体
    RequestDataBuilder& body(const std::string& body);

    // 设置请求头
    RequestDataBuilder& header(const std::string& key, const std::string& value);

    // 生成
    std::string build();
};

// 请求体包装类
class Responser {
    // 响应头
    std::map<std::string, std::string> _headers;
    // 响应体
    std::string _body;

public:
    explicit Responser();

    // 全部解析
    void parseAll(const std::string& result);
    // 解析请求头
    size_t parseHeader(const std::string& data);
    // 解析请求体
    void parseBody(const std::string& data);

    // 获取响应头
    std::string getHeader(const std::string &key);
    // 获取响应体
    std::string getBody();
};

// http响应解析器
class HttpResponserParser {
protected:
    // 正在发送请求行
    //static constexpr uint8_t SENDING_LINE = 1;
    // 正在发送请求头
    static constexpr uint8_t SENDING_HEADERS = 2;
    // 正在发送请求体
    static constexpr uint8_t SENDING_BODY = 3;
    // 已解析完毕
    static constexpr uint8_t PARSE_OVER = 5;

    // 响应接收缓冲区
    std::string responserBuffer;
    // 响应接受状态
    uint8_t state = SENDING_HEADERS;
    // 响应体长度
    int contentLength = 0;
    // 响应体容器
    Responser responser;

    // 响应回调
    static err_t respCallBack(void* arg, tcp_pcb* pb, pbuf *p, err_t err);

public:
    // 不绑定tcp回调
    explicit HttpResponserParser();

    bool checkHttpOver();

    void appendResponserBuffer(const std::string &buffer);

    void requestSendOver();

    Responser getResponser();

    ~HttpResponserParser() = default;
};

// http客户端
class HttpClient {
    // 服务器ip
    std::string _ip;
    // 服务器端口
    int _port = 80;
    // 地址
    ip_addr_t _addr{};
    // 是否连接成功
    bool connected = false;

    // pcb
    tcp_pcb* pcb;
    // 响应解析器
    std::unique_ptr<HttpResponserParser> responserParser;

public:
    virtual ~HttpClient() = default;

    HttpClient(std::string ip, int port);

    // 发送一个请求
    virtual Responser send(const std::string &request);

    // 连接完毕回调
    static err_t connectCallBack(void *arg, tcp_pcb *pcb, err_t err);

    // 请求回调
    static err_t httpRespCallBack(void *arg, tcp_pcb *pb, pbuf *p, err_t err);
};

class HttpsClient {
    // 服务器host
    std::string _host;
    // 服务器端口
    int _port = 80;
    // 地址
    ip_addr_t _addr{};
    // 当前状态
    HttpClientState state = HttpClientState::UN_INIT;

    // pcb
    tcp_pcb* pcb;
    // 响应解析器
    std::unique_ptr<HttpResponserParser> responserParser;

    // TCL连接
    mbedtls_ssl_context ssl{};
    // TLS配置
    mbedtls_ssl_config config{};
    // CA根证书
    mbedtls_x509_crt cacert{};
    // 递源
    mbedtls_entropy_context entropy{};
    // 随机数生成器
    mbedtls_ctr_drbg_context ctrDrbg{};

    // 服务器返回的加密数据
    std::string unDecryResponser;

    // TLS握手
    void tlsHand();

    // 发送加密后的数据
    static int sendTLS(void *ctx, const unsigned char *buf, size_t len);

    // 获得响应解密前的数据
    static int recv(void *ctx, unsigned char *buf, size_t len);

    // 请求回调
    static err_t httpsRespCallBack(void *arg, tcp_pcb *pb, pbuf *p, err_t err);

    // 连接完毕回调
    static err_t connectCallBack(void *arg, tcp_pcb *pcb, err_t err);

    // 致命错误回调
    static void httpsErrorCallBack(void* arg, err_t err);
public:
    HttpsClient(const std::string &host, int port, const char* ca);

    ~HttpsClient();

    // 发送一个请求
    Responser send(const std::string& request);

    // 检查是否正常响应
    [[nodiscard]] bool checkResponserIsOk() const;

    // 检查初始化是否正确
    [[nodiscard]] bool checkInitSuccess() const;
};

#endif