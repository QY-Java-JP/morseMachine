#ifndef MYPROJECT_TIMEUTIL_H
#define MYPROJECT_TIMEUTIL_H

#include "Logger.h"
#include "util/WifiManager.h"
#include "lwip/apps/sntp.h"

class TimeUtil {
    // 是否对齐了时间
    inline static bool flushedTime = false;
public:
    // 将时间对齐现实时间 (使用前必须先连接WIFI)
    static bool flushSystemTime() {
        // 如果没连接WIFI 直接返回
        if (!WifiManager::isConnected()) {
            Logger::debug("TimeUtil", "Wifi not connect");
            return false;
        }

        // 初始化
        setenv("TZ", "CST-8", 1);
        tzset();

        // 初始化snp服务器
        sntp_stop();
        ip_addr_t addr;
        ipaddr_aton("203.107.6.88", &addr);
        sntp_setserver(0, &addr);
        sntp_init();

        // 等待时间同步完毕
        time_t now = 0;
        for (int i = 0; i < 30; i++) {
            Logger::debug("TimeUtil", "Waiting for NTP...");

            cyw43_arch_poll();
            sleep_ms(500);
            time(&now);
            if (now > 24 * 3600) {
                Logger::debug("TimeUtil", "time success");
                flushedTime = true;
                return true;
            }
        }

        // 运行到这里代表超时了
        Logger::debug("TimeUtil", "time fail");
        flushedTime = false;
        return false;
    }

    // 获取当前时间
    static tm getNow() {
        if (!flushedTime) flushSystemTime();

        time_t now;
        tm timeInfo;
        time(&now);
        localtime_r(&now, &timeInfo);

        return timeInfo;
    }
};

#endif //MYPROJECT_TIMEUTIL_H