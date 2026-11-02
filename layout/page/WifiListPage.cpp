#include "WifiListPage.h"

#include "GlobalState.h"
#include "WifiConnectPage.h"

WifiListPage::WifiListPage(PageStack &stack) : OptionPage(stack) { }

std::string WifiListPage::getTitle() {
    return "== Select Wifi ==";
}

void WifiListPage::initPage() {
    // 扫描 保存
    WifiManager::scanWifi(wifiList);
}

void WifiListPage::getAllOptions(std::vector<Option> &allOption, const size_t height) {
    // 选项就是wifi
    // SSID
    const size_t pageSize = height - 1;
    const uint32_t start = (page - 1) * pageSize;
    for (uint32_t i = start; i < start + std::min(wifiList.size(), pageSize); i++) {
        const WifiItem& item = wifiList[i];

        allOption.emplace_back(Option{i, 0, item.ssid});
    }

    // 是不是最后一页
    if (allOption.size() != height - 1) isLastPage = true;
}

bool WifiListPage::selectOption(Option &option) {
    // 找到选中的 存入id 跳转页面
    const WifiItem& item = wifiList[option.id];
    GlobalState::WIFI_SELECTED_SSID = item.ssid;

    Page* newPage = new WifiConnectPage(stack);
    stack.pushPage(*newPage);

    return false;
}

bool WifiListPage::userInput00(std::string key) {
    // 这个页面没什么其他可输入的
    return false;
}


void WifiListPage::onWrapAround() {
    if (isLastPage) return;

    page++;
}