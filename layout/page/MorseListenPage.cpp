#include "MorseListenPage.h"

#include "GlobalState.h"
#include "InputCodePage.h"
#include "PageConstant.h"
#include "layout/net/RequestUtil.h"

MorseListenPage::MorseListenPage(PageStack &stack) : Page(stack) {
    Buzzer* bz = static_cast<Buzzer *>(stack.shareDataMap.find(PageConstant::BUZZER_KEY)->second);
    buzzer = bz;
}

void MorseListenPage::initPage() {
    // 获取morse 解析
    const uint32_t id = GlobalState::MORSE_LISTEN_MORSE_ID;
    GlobalState::MORSE_LISTEN_MORSE_ID = 0;
    if (id == 0) return;

    std::string code16;
    RequestUtil::getMorse(id, code16);
    if (code16.empty()) return;

    parseCode16(code16);
}

void MorseListenPage::pageFlush() {
    if (playOver) return;

    // 记录时间
    showMs += PageStack::CHECK_INPUT_MS;
    // 把当前时间 / 一次记录的时间 最后的值就是当前的code
    const size_t codeIndex = showMs / InputCodePage::WRITE_CODE_DET_MS;
    if (codeIndex >= morse.size()) {
        playOver = true;
        stack.flushTopPage();
        return;
    }

    // 设置蜂鸣器
    if (morse[codeIndex]) {
        buzzer->on();
    } else {
        buzzer->off();
    }
}


bool MorseListenPage::userInput0(std::string key) {
    // 没什么可输入的
    return false;
}

void MorseListenPage::printPage(PageData &page) {
    page.lines[0] = "=== Please Listen ===";
    if (playOver) {
        page.lines[page.maxHeight - 1] = "=== Over ===";
    } else {
        page.lines[page.maxHeight - 1] = "=== Buzzing ===";
    }
}

// 16进制字符串转为二进制存起来
void MorseListenPage::parseCode16(const std::string &code) {
    morse.reserve(code.size() * 4);
    for (const char c : code) {
        uint8_t value;
        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } else {
            // 非法字符：这里选择跳过
            continue;
        }

        for (int i = 3; i >= 0; --i) {
            morse.push_back((value & (1u << i)) != 0);
        }
    }
}



