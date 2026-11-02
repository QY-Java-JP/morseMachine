#ifndef MYPROJECT_WIFIUTIL_H
#define MYPROJECT_WIFIUTIL_H

#include <algorithm>

#include "Logger.h"
#include "pico/cyw43_arch.h"

enum class WifiAuth : uint32_t {
    OPEN = CYW43_AUTH_OPEN,
    WPA = CYW43_AUTH_WPA_TKIP_PSK,
    WPA2 = CYW43_AUTH_WPA2_AES_PSK,
    WPA2_MIXED = CYW43_AUTH_WPA2_MIXED_PSK,
    WPA3 = CYW43_AUTH_WPA3_SAE_AES_PSK,
    WPA2_WPA3 = CYW43_AUTH_WPA3_WPA2_AES_PSK,
};

// Wifi
struct WifiItem {
    std::string ssid;
    int rssi;
    WifiAuth auth;
};

class WifiManager {
    // 是否初始化过WIFI组件
    inline static bool inited = false;
    // 是否已经连接上了wifi
    inline static bool connected = false;

    // 初始化
    static bool init() {
        if (inited) return true;
        if (cyw43_arch_init()) return false;

        cyw43_arch_enable_sta_mode();
        Logger::info("WifiManager", "WifiManager inited");
        inited = true;

        return true;
    }

    // uint32_t的数字转为类型枚举
    static WifiAuth toAuth(const uint32_t value) {
        switch (value) {
            case CYW43_AUTH_OPEN:
                return WifiAuth::OPEN;
            case CYW43_AUTH_WPA_TKIP_PSK:
                return WifiAuth::WPA;
            case CYW43_AUTH_WPA2_AES_PSK:
                return WifiAuth::WPA2;
            case CYW43_AUTH_WPA2_MIXED_PSK:
                return WifiAuth::WPA2_MIXED;
            case CYW43_AUTH_WPA3_SAE_AES_PSK:
                return WifiAuth::WPA3;
            case CYW43_AUTH_WPA3_WPA2_AES_PSK:
                return WifiAuth::WPA2_WPA3;
            default:
                // 未知值的处理需要另行设计
                return WifiAuth::OPEN;
        }
    }

public:
    // 扫描附近的wifi
    static void scanWifi(std::vector<WifiItem>& list) {
        // 开启扫描
        if (!init()) return;

        std::vector<WifiItem> completeList;
        cyw43_wifi_scan_options_t options = {};
        const int err = cyw43_wifi_scan(&cyw43_state, &options, &completeList, wifiScanResult);
        if (err != ERR_OK) {
            Logger::error("WifiManager", "扫描开启失败 errCode: " + std::to_string(err));
            return;
        }

        // 扫描直到结束
        while (cyw43_wifi_scan_active(&cyw43_state)) {
            cyw43_arch_poll();
            sleep_ms(1);
        }

        // 按照信号强度排个序
        std::sort(completeList.begin(), completeList.end(), [](const auto& a, const auto& b) {
            return a.rssi > b.rssi;
        });

        // 做一个去重 因为bssid不一样但ssid一样的会被扫描多次
        for (const auto& wifi : completeList) {
            auto it = std::find_if(list.begin(), list.end(), [&](const auto& item) {
                return item.ssid == wifi.ssid;
            });

            if (it == list.end()) list.emplace_back(wifi);
        }
    }

    // 扫描结果回调
    static int wifiScanResult(void* env, const cyw43_ev_scan_result_t* result) {
        auto* retList = static_cast<std::vector<WifiItem>*>(env);
        if (result == nullptr) return ERR_OK;

        WifiItem item = {
            std::string(reinterpret_cast<const char*>(result->ssid), result->ssid_len),
            result->rssi,
            toAuth(result->auth_mode)
        };

        Logger::debug("WifiManager", "扫描到: " + item.ssid);
        retList->emplace_back(item);

        return ERR_OK;
    }

    // 连接
    static bool connectWifi(const std::string& ssid, const std::string& password, const WifiAuth auth = WifiAuth::WPA2) {
        // 初始化
        if (!init()) return false;

        // 连接wifi
        const int errCode = cyw43_arch_wifi_connect_timeout_ms(
        ssid.c_str(), password.c_str(),
            static_cast<uint32_t>(auth), 10000
        );

        if (errCode != ERR_OK) {
            Logger::info("WifiManager", "Wifi connect result = " + std::to_string(errCode));
            connected = false;
            return false;
        }

        Logger::info("WifiManager", "Wifi connect success");
        connected = true;
        return true;
    }

    // 是否已经连接上了wifi
    static bool isConnected(){
        return connected;
    }
};

#endif //MYPROJECT_WIFIUTIL_H