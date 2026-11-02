#ifndef MORSEMACHINE_HINTOPTIONSPAGE_H
#define MORSEMACHINE_HINTOPTIONSPAGE_H

#include "OptionPage.h"
#include "TaskScheduler.hpp"

class HintOptionsPage : public OptionPage{
    // 是否可以在显示提示的时候响应输入
    bool onHintCanHandlerInput = false;
    // hint显示在倒数第几行
    uint8_t hintShowLineIndexCountDown = 2;
    // 提示是否正在显示
    bool hintShowing = false;
    // 提示显示的持续时间
    uint32_t hintShowingMs = 0;

    // 是否需要等待完毕返回页面
    bool showOverBackPage = false;

    // 提示信息
    std::string hint = "Hint";

    // 关闭提示
    void offHint() {
        hintShowing = false;
        hintShowingMs = 0;
        setCanBack(true);
        Logger::debug("HintOptionsPage", "隐藏提示");

        if (showOverBackPage) {
            showOverBackPage = false;
            stack.back();
        } else {
            stack.flushTopPage();
        }
    }

protected:
    // 修改是否...
    void setOnHitCanHandlerInput(const bool can) {
        onHintCanHandlerInput = can;
    }

    // 修改显示的行数
    void setHintShowLineIndex(const uint8_t index) {
        hintShowLineIndexCountDown = index;
    }

    // 显示提示
    void showHint(const std::string& newHint) {
        hint = newHint;
        hintShowing = true;
        setCanBack(false);
        Logger::debug("HitOptionsPage", "显示提示: " + hint);

        stack.flushTopPage();
    }

    // 提示完毕后返回页面
    void showHintOverBackPage(const std::string& newHint) {
        showHint(newHint);
        showOverBackPage = true;
    }

public:
    // hint显示时间
    static inline uint32_t HINT_SHOW_MS = 1000 * 2;

    explicit HintOptionsPage(PageStack& stack) : OptionPage(stack) { }

    bool userInput0(std::string key) override {
        // 如果不能响应则拦截住
        if (hintShowing && !onHintCanHandlerInput) return false;

        return OptionPage::userInput0(key);
    }

    // 刷新页面
    void pageFlush() override {
        // 如果正在显示提示 增加时间
        if (hintShowing) hintShowingMs += PageStack::CHECK_INPUT_MS;

        // 如果时间到了 调用
        if (hintShowingMs > HINT_SHOW_MS) {
            offHint();
        }
    }

    // 重写父类打印
    void printPage(PageData &page) override {
        // 我们先让父类修改完 然后我们在规定的行数覆盖提示
        OptionPage::printPage(page);
        if (hintShowing) {
            page.lines[page.maxHeight - hintShowLineIndexCountDown] = hint;
        }
    }
};

#endif //MORSEMACHINE_HINTOPTIONSPAGE_H