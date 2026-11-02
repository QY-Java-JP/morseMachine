#include "SendMorsePage.h"

#include "GlobalState.h"
#include "InputCodePage.h"
#include "InputPage.h"
#include "RegisterPage.h"
#include "layout/net/RequestUtil.h"

SendMorsePage::SendMorsePage(PageStack &stack) : HintOptionsPage(stack) { }

SendMorsePage::~SendMorsePage() {
    code->clear();
}


std::string SendMorsePage::getTitle() {
    return "=== Send Morse ===";
}

void SendMorsePage::showPage() {
    // 看看是不是从inputPage回来了
    if (otherPageBackInputName == "acceptName") {
        // 检查下这个用户是否存在
        const JsonDocument doc = RequestUtil::getSimpleUser(GlobalState::INPUT_PAGE_TEXT);
        if (doc["code"].as<int>() == 10003) {
            showHint("Name Not Find");
            GlobalState::INPUT_PAGE_TEXT = "";
            return;
        }

        acceptUser.userId = doc["data"]["userId"].as<int>();
        acceptUser.userName = GlobalState::INPUT_PAGE_TEXT;
        GlobalState::INPUT_PAGE_TEXT = "";
    }
}


void SendMorsePage::getAllOptions(std::vector<Option> &allOption, size_t height) {
    // acceptName code ok
    allOption.emplace_back(Option{1, 0, "AcceptName: " + acceptUser.userName});
    allOption.emplace_back(Option{2, 0,"Code: " + (code->empty() ? std::string("") : std::string(".."))});
    allOption.emplace_back(Option{3, 1, "OK"});
}

bool SendMorsePage::selectOption(Option &option) {
    // acceptName
    if (option.text.compare(0, 6, "Accept") == 0) {
        // 设置长度和内容 跳转
        GlobalState::INPUT_PAGE_TEXT = acceptUser.userName;
        GlobalState::INPUT_PAGE_TEXT_MAX_SIZE = RegisterPage::UNAME_MAX_SIZE;
        otherPageBackInputName = "acceptName";

        Page* newPage = new InputPage(stack);
        stack.pushPage(*newPage);
    }

    // 如果点击了Code
    if (option.text.compare(0, 4, "Code") == 0) {
        otherPageBackInputName = "code";
        code->clear();

        Page* newPage = new InputCodePage(stack);
        stack.pushPage(*newPage);
    }

    // 如果点击了OK
    if (option.text == "OK") {
        // 先看看是不是都输入了
        if (acceptUser.userName.empty() || code->empty()) {
            showHint("Miss Data");
            return false;
        }

        // 发送
        const std::string code16 = parseCodeTo16();
        Logger::info("SendMorsePage", "发送: " + code16);
        if (RequestUtil::sendMorse(acceptUser.userId, code16)) {
            showHintOverBackPage("Success!");
            return false;
        }
    }

    return false;
}

bool SendMorsePage::userInput00(std::string key) {
    return false;
}

// 把code集合中的数据转为16进制的字符串
std::string SendMorsePage::parseCodeTo16() const {
    static constexpr char HEX[] = "0123456789ABCDEF";

    std::string result;
    for (size_t i = 0; i < code->size(); i += 4) {
        uint8_t value = 0;

        for (size_t j = 0; j < 4; ++j) {
            value <<= 1;
            if (i + j < code->size() && (*code)[i + j]) {
                value |= 1;
            }
        }

        result += HEX[value];
    }

    return result;
}





