#ifndef MORSEMACHINE_WIFICONNECTPAGE_H
#define MORSEMACHINE_WIFICONNECTPAGE_H

#include "layout/HintOptionsPage.h"

class WifiConnectPage : public HintOptionsPage {
    // ssid
    std::string ssid;
    // 密码
    std::string pwd;

    // 进入input页面时候输入的东西
    std::string inputPageInputDataName = "pwd";

    void getAllOptions(std::vector<Option> &allOption, size_t height) override;

    bool selectOption(Option &option) override;

    bool userInput00(std::string key) override;

    void showPage() override;

    std::string getTitle() override;

public:
    // 密码最大长度
    static inline uint8_t PWD_MAX_SIZE = 20;

    explicit WifiConnectPage(PageStack& stack);
};


#endif //MORSEMACHINE_WIFICONNECTPAGE_H