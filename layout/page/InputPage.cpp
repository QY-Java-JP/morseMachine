#include "InputPage.h"

#include "GlobalState.h"
#include "PageConstant.h"
#include "layout/PageStack.h"
#include "util/Logger.h"

InputPage::InputPage(PageStack &page_stack) : Page(page_stack) {
    Buzzer* bz = static_cast<Buzzer *>(stack.shareDataMap.find(PageConstant::BUZZER_KEY)->second);
    buzzer = bz;
}

void InputPage::initPage() {
    // 重置长度和数据
    maxSize = GlobalState::INPUT_PAGE_TEXT_MAX_SIZE;
    inputString = GlobalState::INPUT_PAGE_TEXT;
}

void InputPage::hidePage() {
    // 存起来
    GlobalState::INPUT_PAGE_TEXT = inputString;
    GlobalState::INPUT_PAGE_TEXT_MAX_SIZE = maxSize;
}


void InputPage::pageFlush() {
    // 增加时间
    intervalMs += static_cast<uint32_t>(PageStack::CHECK_INPUT_MS);

    // 如果当前状态是弹起
    if (!buzzer->isOn()) {
        if (!pushDataHandled) {
            // 根据时间不同判断是长还是短 存起来
            inputMorse += std::to_string(intervalMs > 120 ? 1 : 0);
            Logger::debug("InputPage", "电码变为: " + inputMorse);

            pushDataHandled = true;
            intervalMs = 0;
        }

        // 看看持续的时间是不是超过了2s 如果超过了进行解码
        if (intervalMs > 700) {
            parseInputMorse();
        }

    }
}


void InputPage::printPage(PageData &page) {
    // 第一行 Please Input
    // 第二行 "“
    // 第三行开始是内容
    page.lines[0] = "Please Input("+std::to_string(inputString.length())+"/"+std::to_string(maxSize)+")";
    page.lines[1] = "";

    for (int i = 0; i < inputString.length(); i += page.maxWidth) {
        page.lines[i+2] = inputString.substr(i, page.maxWidth);
    }
}

bool InputPage::userInput0(const std::string key) {
    // 如果是按下则 重置按下时间 记录按下
    if (key == PageConstant::MORSE_PUSH) {
        intervalMs = 0;
        buzzer->on();
        setCanBack(false);
    }

    // 如果是弹起 记录弹起
    if (key == PageConstant::MORSE_UP) {
        pushDataHandled = false;
        buzzer->off();
        setCanBack(true);
    }

    return false;
}

// 解码当前的电报码
void InputPage::parseInputMorse() {
    if (inputMorse.empty()) return;

    // 去表中找对应
    auto it = PageConstant::MORSE_TABLE.find(inputMorse);
    if (it != PageConstant::MORSE_TABLE.end()) {
        inputString += it->second;
        Logger::debug("InputPage", "电码变为: " + inputString);

    // 看看是不是需要撤销
    } else if (inputMorse == "000000") {
        // 撤销上一个字符
        if (!inputString.empty()) {
            inputString.pop_back();
            Logger::debug("InputPage", "电码撤销一个字符");
        }
    } else {
        // 未知电报码
        inputString += "?";
        Logger::debug("InputPage", "未知电码输入");
    }

    // 最后看下是不是超过了最大长度 如果超过了截掉
    inputString = inputString.substr(0, maxSize);

    inputMorse = "";
    stack.flushTopPage();
}



