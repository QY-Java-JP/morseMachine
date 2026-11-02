#include "LoginPage.h"

#include "GlobalState.h"
#include "InputPage.h"
#include "layout/net/RequestUtil.h"

LoginPage::LoginPage(PageStack &stack) : HintOptionsPage(stack) {
}

std::string LoginPage::getTitle() {
    return "== Enter Info ==";
}

void LoginPage::showPage() {
    // 是不是输入完毕回来了
    if (inputPageInputDataName == "uname") {
        uname = GlobalState::INPUT_PAGE_TEXT;
    }

    if (inputPageInputDataName == "pwd") {
        pwd = GlobalState::INPUT_PAGE_TEXT;
    }

    GlobalState::INPUT_PAGE_TEXT = "";
}


void LoginPage::getAllOptions(std::vector<Option> &allOption, size_t height) {
    // name password ok
    allOption.emplace_back(Option{1, 0, "name: " + uname});
    allOption.emplace_back(Option{2, 0, "password: " + (pwd.empty() ? std::string("") : std::string("***"))});
    allOption.emplace_back(Option{3, 1, "OK"});
}

bool LoginPage::selectOption(Option &option) {
    // 如果点击了name
    if (option.text.compare(0, 4, "name") == 0) {
        // 设置长度和内容 跳转
        GlobalState::INPUT_PAGE_TEXT = uname;
        GlobalState::INPUT_PAGE_TEXT_MAX_SIZE = UNAME_MAX_SIZE;
        inputPageInputDataName = "uname";

        Page* newPage = new InputPage(stack);
        stack.pushPage(*newPage);
    }

    // 如果点击了password
    if (option.text.compare(0, 8, "password") == 0) {
        // 设置长度和内容 跳转
        GlobalState::INPUT_PAGE_TEXT = pwd;
        GlobalState::INPUT_PAGE_TEXT_MAX_SIZE = UPWD_MAX_SIZE;
        inputPageInputDataName = "pwd";

        Page* newPage = new InputPage(stack);
        stack.pushPage(*newPage);
    }

    // 如果点击了OK
    if (option.text == "OK") {
        // 如果uname或pwd有一个空
        if (uname.empty() || pwd.empty()) {
            showHint("Miss Data");
            return false;
        }

        // 那就登录
        const JsonDocument doc = RequestUtil::login(uname, pwd);
        const int code = doc["code"].as<int>();
        if (code == 200) {
            // 登录成功了 就返回
            GlobalState::TOKEN = doc["data"].as<std::string>();

            showHintOverBackPage("Success!");
            return false;
        }
        if (code == 10002) {
            showHint("Password Wrong");
            return false;
        }
    }

    return false;
}

bool LoginPage::userInput00(std::string key) {
    return false;
}


