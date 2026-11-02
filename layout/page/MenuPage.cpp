#include "MenuPage.h"

#include <layout/PageStack.h>

#include "GlobalState.h"
#include "InputPage.h"
#include "LoginPage.h"
#include "MessageListPage.h"
#include "RegisterPage.h"
#include "SendMorsePage.h"

MenuPage::MenuPage(PageStack &page_stack) : OptionPage(page_stack) {
}

void MenuPage::getAllOptions(std::vector<Option>& allOption, size_t height) {
    // 如果用户没有登录 则显示的是 Register Login
    if (GlobalState::TOKEN.empty()) {
        allOption.emplace_back(Option{1, 0, "Login"});
        allOption.emplace_back(Option{2, 1, "Register"});
    }

    // 如果登录了 则显示 SendMsg MsgList
    if (!GlobalState::TOKEN.empty()) {
        allOption.emplace_back(Option{3, 0, "SendMsg"});
        allOption.emplace_back(Option{4, 1, "MsgList"});
    }
}

bool MenuPage::selectOption(Option& option) {
    if (option.text == "Register") {
        Page* registerPage = new RegisterPage(stack);
        stack.pushPage(*registerPage);
        return false;
    }

    if (option.text == "Login") {
        Page* loginPage = new LoginPage(stack);
        stack.pushPage(*loginPage);
        return false;
    }

    if (option.text == "SendMsg") {
        Page* sendMsgPage = new SendMorsePage(stack);
        stack.pushPage(*sendMsgPage);
        return false;
    }

    if (option.text == "MsgList") {
        Page* msgListPage = new MessageListPage(stack);
        stack.pushPage(*msgListPage);
        return false;
    }

    return false;
}


bool MenuPage::userInput00(const std::string key) {
    return false;
}

std::string MenuPage::getTitle() {
    // 如果登录了显示Welcome
    return GlobalState::TOKEN.empty() ? "" : "=== Welcome ===";
}