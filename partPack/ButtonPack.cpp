#include "ButtonPack.h"

#include "../TaskScheduler.hpp"
#include "pico/stdlib.h"
#include "util/Logger.h"

ButtonPack::ButtonPack(const uint8_t pid) {
    // 初始化按钮
    gpio_init(pid);
    gpio_set_dir(pid, GPIO_IN);
    gpio_pull_up(pid);
    this->pid = pid;
    Logger::info("ButtonPack", "针脚: " + std::to_string(pid) + "绑定按钮");
}

// 初始化
void ButtonPack::initCallBack() {
    // 把执行方法绑定到定时事件中 50ms检查一次
    if (onPress == nullptr && onLongPress == nullptr) return;
    if (callbackInited) return;

    TaskScheduler::init();
    TaskScheduler::setInterval([this]{run();}, INTERVAL_MS);
    callbackInited = true;
}

// 执行的方法
void ButtonPack::run() {
    // 如果没有按下 判断按下的时长
    if (gpio_get(pid)) {
        // 如果大于1.5s 则就是长按 大于100ms 就是短按
        if (clickedMs > 500 && onLongPress != nullptr) {
            onLongPress(*this);
        } else if (clickedMs > 50 && onPress != nullptr) {
            onPress(*this);
        }

        clickedMs = 0;
        return;
    }

    // 如果按下了 积累时间
    clickedMs += INTERVAL_MS;
}

// 绑定单击
void ButtonPack::registerOnPress(void (*callback)(ButtonPack&)) {
    this->onPress = callback;
    initCallBack();
    Logger::debug("ButterPack", "按钮单击 针脚: " + std::to_string(this->pid));
}

// 绑定长按
void ButtonPack::registerOnLongPass(void (*callback)(ButtonPack &)) {
    this->onLongPress = callback;
    initCallBack();
    Logger::debug("ButterPack", "按钮长按 针脚: " + std::to_string(this->pid));
}

