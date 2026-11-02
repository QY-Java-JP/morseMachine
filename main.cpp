
#include <layout/PageStack.h>

#include "pico/stdlib.h"
#include "TaskScheduler.hpp"
#include "layout/net/RequestUtil.h"
#include "layout/page/PageConstant.h"
#include "layout/page/WifiListPage.h"
#include "partPack/Buzzer.h"
#include "partPack/Joystick.h"

void x(Joystick& joystick, uint16_t x, bool center) {
    auto* stack = joystick.getAttr<PageStack>("pageStack");
    if (x > 3000) {
        stack->setInput(Page::NEXT_BTN_PUSH);
    } else if (x < 1000) {
        stack->setInput(Page::BACK_BTN_PUSH);
    }
}

void y(Joystick& joystick, uint16_t y, bool center) {
    // 看看方向
    auto* stack = joystick.getAttr<PageStack>("pageStack");
    if (center) {
        stack->setInput(PageConstant::MORSE_UP);
    } else {
        stack->setInput(PageConstant::MORSE_PUSH);
    }
}

void push(Joystick& joystick) {
    auto* stack = joystick.getAttr<PageStack>("pageStack");
    stack->setInput(Page::OK_BTN_PUSH);
}

int main() {
    // 初始化基本
    stdio_init_all();
    Logger::setLevel(LoggerLevel::DEBUG);
    TaskScheduler::init(false);


    // 初始化组件
    OledPack oled = OledPack(16, 17, 8, 16);
    PageStack stack = PageStack(oled);

    Joystick stick = Joystick(22, 0, 1);
    stick.registerXChange(x);
    stick.registerYChange(y);
    stick.registerPush(push);
    stick.addAttr("pageStack", &stack);

    Buzzer buzzer = Buzzer(15);
    stack.shareDataMap[PageConstant::BUZZER_KEY] = &buzzer;

    Page* indexPage = new WifiListPage(stack);
    stack.start(*indexPage);

    while (true) {
        TaskScheduler::syncRun();
        sleep_ms(TaskScheduler::LOOP_WAIT_MS);
    }
}