#ifndef MORSEMACHINE_JOYSTICK_H
#define MORSEMACHINE_JOYSTICK_H

#include <map>
#include <string>

#include "TaskScheduler.hpp"
#include "hardware/adc.h"
#include "pico/types.h"

class Joystick {
    // 一次判断的时间ms
    static constexpr int INTERVAL_MS = 50;

    // 针脚号
    uint _swPin = 0;
    uint _xAdc = 0;
    uint _yAdc = 0;

    // 附加属性
    std::map<std::string, void*> attr;
    // 回调事件是否绑定完毕
    bool callbackInited = false;

    // 按钮是否按下
    bool pushing = false;
    // x是否正在被移动
    bool xMoved = false;
    // y是否正在被移动
    bool yMoved = false;
    // 是否开启xy有变化就持续回调
    bool xYRepeatCallback = false;

    // 回调
    void (*onXChange)(Joystick&, uint16_t, bool) = nullptr;
    void (*onYChange)(Joystick&, uint16_t, bool) = nullptr;
    void (*onPush) (Joystick&) = nullptr;

    void run() {
        // 读取 X 轴
        adc_select_input(_xAdc);
        const uint16_t x = adc_read();
        const bool xNoCenter = std::abs(static_cast<int>(x) - CENTER) > DEAD_ZONE;

        // 读取 Y 轴
        adc_select_input(_yAdc);
        const uint16_t y = adc_read();
        const bool yNoCenter = std::abs(static_cast<int>(y) - CENTER) > DEAD_ZONE;

        // X 轴：进入非死区、持续移动、回到死区
        if (xNoCenter) {
            if (xYRepeatCallback || !xMoved) {
                if (onXChange != nullptr) {
                    onXChange(*this, x, false);
                }
            }
        } else if (xMoved) {
            // X 轴刚刚回到死区
            if (onXChange != nullptr) {
                onXChange(*this, x, true);
            }
        }
        xMoved = xNoCenter;

        // Y 轴：进入非死区、持续移动、回到死区
        if (yNoCenter) {
            if (xYRepeatCallback || !yMoved) {
                if (onYChange != nullptr) {
                    onYChange(*this, y, false);
                }
            }
        } else if (yMoved) {
            // Y 轴刚刚回到死区
            if (onYChange != nullptr) {
                onYChange(*this, y, true);
            }
        }
        yMoved = yNoCenter;

        // 按钮：按下沿触发一次
        if (!gpio_get(_swPin)) {
            if (!pushing) {
                if (onPush != nullptr) {
                    onPush(*this);
                }
                pushing = true;
            }
        } else {
            pushing = false;
        }
    }

    // 初始化回调
    void initCallback() {
        if (callbackInited) return;

        TaskScheduler::init();
        TaskScheduler::setInterval([this] {run();}, INTERVAL_MS);
        callbackInited = true;
    }

public:
    // 死区
    static constexpr uint16_t CENTER = 2048;
    static constexpr uint16_t DEAD_ZONE = 500;

    Joystick(const uint swPin, const uint xAdc, const uint yAdc) {
        _swPin = swPin;
        _xAdc = xAdc;
        _yAdc = yAdc;

        adc_init();
        adc_gpio_init(_xAdc);
        adc_gpio_init(_yAdc);
        gpio_init(_swPin);
        gpio_set_dir(_swPin, GPIO_IN);
        gpio_pull_up(_swPin);
    }

    // 设置是否xy改变了就持续回调
    void setXYRepeatCallback(const bool on) {
        xYRepeatCallback = on;
    }

    // 注册x改变
    void registerXChange(void (*callback)(Joystick&, uint16_t, bool)) {
        onXChange = callback;
        initCallback();
    }

    // 注册y改变
    void registerYChange(void (*callback)(Joystick&, uint16_t, bool)) {
        onYChange = callback;
        initCallback();
    }

    // 注册按钮压下
    void registerPush(void (*callback)(Joystick&)) {
        onPush = callback;
        initCallback();
    }

    // 添加参数
    template<typename T>
    void addAttr(const std::string& key, T* value) {
        attr[key] = static_cast<void*>(value);
    }

    // 删除参数
    void deleteAttr(const std::string& key) {
        attr.erase(key);
    }

    // 获取参数
    template<typename T>
    T* getAttr(const std::string &key) {
        return static_cast<T*>(attr[key]);
    }
};

#endif //MORSEMACHINE_JOYSTICK_H