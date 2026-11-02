#include "WifiConnectPage.h"

#include "GlobalState.h"
#include "InputPage.h"
#include "MenuPage.h"
#include "layout/net/RequestUtil.h"
#include "util/WifiManager.h"

WifiConnectPage::WifiConnectPage(PageStack &stack) : HintOptionsPage(stack) {
    ssid = GlobalState::WIFI_SELECTED_SSID;
}

std::string WifiConnectPage::getTitle() {
    return "== Enter Pwd ==";
}

void WifiConnectPage::showPage() {
    // 是不是输入完毕回来了
    if (inputPageInputDataName == "pwd") {
        pwd = GlobalState::INPUT_PAGE_TEXT;
    }

    GlobalState::INPUT_PAGE_TEXT = "";
}


void WifiConnectPage::getAllOptions(std::vector<Option> &allOption, size_t height) {
    // ssid password ok
    allOption.emplace_back(Option{1, 0, "ssid: " + ssid});
    allOption.emplace_back(Option{2, 0, "password: " + (pwd.empty() ? std::string("") : std::string("***"))});
    allOption.emplace_back(Option{3, 1, "OK"});
}

bool WifiConnectPage::selectOption(Option &option) {
    // 如果点击了name
    if (option.text.compare(0, 4, "name") == 0) {
        // 不需要反应
        return false;
    }

    // 如果点击了password
    if (option.text.compare(0, 8, "password") == 0) {
        // 设置长度和内容 跳转
        GlobalState::INPUT_PAGE_TEXT = pwd;
        GlobalState::INPUT_PAGE_TEXT_MAX_SIZE = PWD_MAX_SIZE;
        inputPageInputDataName = "pwd";

        Page* newPage = new InputPage(stack);
        stack.pushPage(*newPage);
    }

    // 如果点击了OK
    if (option.text == "OK") {
        // 如果uname或pwd有一个空
        if (ssid.empty() || pwd.empty()) {
            showHint("Miss Data");
            return false;
        }

        // 那就尝试连接
        const bool ok = WifiManager::connectWifi(ssid, pwd);
        Logger::info("WifiConnectPage", "Wifi连接: " + std::to_string(ok));
        if (ok) {
            // tcl
            RequestUtil::init();

            // 压入菜单
            Page* menuPage = new MenuPage(stack);
            stack.resetTo(*menuPage);
            return false;
        } else {
            showHint("Pwd False");
        }
    }

    return false;
}

bool WifiConnectPage::userInput00(std::string key) {
    return false;
}
