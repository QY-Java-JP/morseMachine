#ifndef MORSEMACHINE_WIFILISTPAGE_H
#define MORSEMACHINE_WIFILISTPAGE_H

#include "layout/OptionPage.h"
#include "util/WifiManager.h"

class WifiListPage : public OptionPage {
    // 所有数据
    std::vector<WifiItem> wifiList;
    // 当前页号
    uint32_t page = 1;
    // 是否到底
    bool isLastPage = false;

    void initPage() override;

    void getAllOptions(std::vector<Option> &allOption, size_t height) override;

    std::string getTitle() override;

    bool selectOption(Option &option) override;

    bool userInput00(std::string key) override;

    void onWrapAround() override;

public:
    explicit WifiListPage(PageStack& stack);

};


#endif //MORSEMACHINE_WIFILISTPAGE_H