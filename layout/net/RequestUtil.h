#ifndef MORSEMACHINE_REQUEST_H
#define MORSEMACHINE_REQUEST_H

#include <string>
#include <ArduinoJson.h>

#include "pico/mutex.h"
#include "layout/page/GlobalState.h"
#include "HttpClient.h"
#include "layout/page/PageConstant.h"
#include "util/Logger.h"

const char ca_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDJjCCAg6gAwIBAgIUXarQQiCLQl6HnFuWk/qQHy+Nm6MwDQYJKoZIhvcNAQEL\n"
    "BQAwFDESMBAGA1UEAwwJbG9jYWxob3N0MB4XDTI2MTAyODA2MTEzM1oXDTI3MTAy\n"
    "ODA2MTEzM1owFDESMBAGA1UEAwwJbG9jYWxob3N0MIIBIjANBgkqhkiG9w0BAQEF\n"
    "AAOCAQ8AMIIBCgKCAQEAzsdPlnARVqMXWXCT8b2SflYq+UIEkWgFCSBhgZK2muoV\n"
    "+3WHU8+Vt33lRsN6tnPIgSUpZnu0fwt1gHNlNhAqL4hcY13YWtnZahKNl9U1Zbu+\n"
    "Vgz+PgeGjYlghkZDmHkuisB/TCimLqS28s5uUEltvfCod2ujlA/OTXaVMB6qltv6\n"
    "ucFdgsOPeoNBiMb2eR3xkFhpaAHAJH/6siOLXyilhXfkx5Q+h81+PO7CVF6DrPaC\n"
    "FhPEBfBWVcOEUJqnJ6pKTHIJ/G/HTDQpuPk0N6NF8YmX71NBC14dI4BTTl+i73pT\n"
    "tm6b5Mc54ufe5EA1T1DqFMu3YXCDa363YSPSfB+qJwIDAQABo3AwbjAMBgNVHRMB\n"
    "Af8EAjAAMA4GA1UdDwEB/wQEAwIFoDATBgNVHSUEDDAKBggrBgEFBQcDATAaBgNV\n"
    "HREEEzARgglsb2NhbGhvc3SHBMCoKBEwHQYDVR0OBBYEFGAczJZDUcmlAbTvRtyj\n"
    "UE7eggoyMA0GCSqGSIb3DQEBCwUAA4IBAQDAGsmMYnprfZdb9k0to+VF3geqiGqV\n"
    "1WYuvdsjquRQm/rQDOZc9UqGrZMJXl9hqBvhJg1UjhCJP+gcjJhGgZDvPFLfjOs+\n"
    "9JrzeaOyCpxz5K0Esscst1k6YCfpXxUU8F7xP4bCpv+pBZ9lmi3EPRSCjlPj9RN2\n"
    "MdBZRHNfkdAY8K+I9EM3rQ1U5egnj+WrKS182mxkOCNWG+0k6aQD5roSguDlrmWV\n"
    "xmZ4iqhpLjIQ4ZA9a2Mm/gcqzxtOCEhjU2GSV9y69ACgCZieTHDhghIC+1pWEGQQ\n"
    "7Vto9Od6k33PuHrCsZZcQ3PyHgcU+HmZD4s5ARblkEbo8/hKpckNUmvb\n"
    "-----END CERTIFICATE-----\n";

// TODO 目前每发一次请求都进行一次 TCP/TLS 握手
class RequestUtil {
public:
    // map锁
    static inline mutex_t responserMutex;
    // doc
    static inline JsonDocument doc{};

    static void init() {
        mutex_init(&responserMutex);
    }

    // 查询用户名是否被占用
    static bool userNameIsUsed(const std::string& name) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/user-name-in-use?userName=" + name)
                .method(RequestMethod::GET)
                .header("Host", "192.168.40.17:8443")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "userNameIsUsed Responser: " + responser);

        deserializeJson(doc, responser.c_str());
        return doc["data"].as<int>() > 0;
    }

    // 注册用户
    static bool registerUser(const std::string& uname, const std::string& pwd) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/register-user")
                .method(RequestMethod::POST)
                .header("Host", "192.168.40.17:8443")
                .header("Content-Type", "application/json")
                .body(R"({"userName": ")" + uname + R"(", "password": ")" + pwd + "\"}")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "registerUser Responser: " + responser);

        deserializeJson(doc, responser.c_str());
        return doc["code"].as<int>() == 200;
    }

    // 登录
    static JsonDocument login(const std::string& uname, const std::string& pwd) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/login")
                .method(RequestMethod::POST)
                .header("Host", "192.168.40.17:8443")
                .header("Content-Type", "application/json")
                .body(R"({"userName": ")" + uname + R"(", "password": ")" + pwd + "\"}")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "login Responser: " + responser);
        deserializeJson(doc, responser.c_str());

        return doc;
    }

    // 查询简单用户
    static JsonDocument getSimpleUser(const std::string& userName) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/simple-user-by-name?userName=" + userName)
                .method(RequestMethod::GET)
                .header("token", GlobalState::TOKEN)
                .header("Host", "192.168.40.17:8443")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "getSimpleUser Responser: " + responser);
        deserializeJson(doc, responser.c_str());

        return doc;
    }

    // 插入一条电报
    static bool sendMorse(const uint64_t acceptId, const std::string& data) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/morse/send-morse")
                .method(RequestMethod::POST)
                .header("Host", "192.168.40.17:8443")
                .header("token", GlobalState::TOKEN)
                .header("Content-Type", "application/json")
                .body(R"({"accepterId": )" + std::to_string(acceptId) + R"(, "data": ")" + data + "\"}")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "sendMorse Responser: " + responser);
        deserializeJson(doc, responser.c_str());

        int code = doc["code"].as<int>();
        return code == 200;
    }

    // 获取电报列表
    static void getMorseList(uint16_t days , std::vector<VSimpleMorse>& morseList) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/morse/now-to-before-days-morse-list?days=" + std::to_string(days))
                .method(RequestMethod::GET)
                .header("token", GlobalState::TOKEN)
                .header("Host", "192.168.40.17:8443")
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "getMorseList Responser: " + responser);
        deserializeJson(doc, responser.c_str());

        if (doc["code"].as<int>() != 200) return;

        const auto morseLs = doc["data"].as<JsonArray>();
        for (JsonObject o : morseLs) {
            VSimpleMorse sm = {
                .morseId = o["morseId"].as<uint32_t>(),
                .sender = {
                    .userId = o["sender"]["userId"].as<uint64_t>(),
                    .userName = o["sender"]["userName"].as<std::string>()
                },
                .state = o["state"].as<uint8_t>(),
                .createTime = o["createTime"].as<std::string>(),
            };

            morseList.emplace_back(sm);
        }
    }

    // 接受电报详情
    static void getMorse(uint32_t morseId, std::string& morse) {
        HttpsClient client{"192.168.40.17", 8443, ca_cert};

        RequestDataBuilder request = RequestDataBuilder();
        const std::string requestStr = request.url("/user/morse/morse-context?morseId=" + std::to_string(morseId))
                .method(RequestMethod::GET)
                .header("Host", "192.168.40.17:8443")
                .header("token", GlobalState::TOKEN)
                .build();

        const std::string responser = client.send(requestStr).getBody();
        Logger::info("RequestUtil", "getMorse Responser: " + responser);
        deserializeJson(doc, responser.c_str());

        if (doc["code"].as<int>() != 200) return;

        morse = doc["data"]["morse"].as<std::string>();
    }

};

#endif // MORSEMACHINE_REQUEST_H