#include "InputCodePage.h"

#include "PageConstant.h"
#include "util/Logger.h"

InputCodePage::InputCodePage(PageStack &stack) : Page(stack) {
    Buzzer* bz = static_cast<Buzzer *>(stack.shareDataMap.find(PageConstant::BUZZER_KEY)->second);
    buzzer = bz;
}

void InputCodePage::pageFlush() {
    // 每刷新五次记一次
    flushCount++;
    if (flushCount >= WRITE_CODE_FLUSH_COUNT_DET) {
        // 检查是压下还是抬起 追加01
        code->emplace_back(buzzer->isOn());
        Logger::debug("InputCodePage", "追加: " + std::to_string(buzzer->isOn()));
        flushCount = 0;
    }
}


void InputCodePage::initPage() {
    // 重置电码
    code->clear();
}

void InputCodePage::printPage(PageData &page) {
    page.lines[0] = "== Please Input ==";
}

bool InputCodePage::userInput0(std::string key) {
    // 压下
    if (key == PageConstant::MORSE_PUSH) {
        buzzer->on();
    }

    // 抬起
    if (key == PageConstant::MORSE_UP) {
        buzzer->off();
    }

    return false;
}


