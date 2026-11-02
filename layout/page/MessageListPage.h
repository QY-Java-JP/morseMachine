#ifndef MORSEMACHINE_MESSAGELISTPAGE_H
#define MORSEMACHINE_MESSAGELISTPAGE_H
#include "PageConstant.h"
#include "layout/OptionPage.h"

class MessageListPage : public OptionPage {
    // 所有数据
    std::vector<VSimpleMorse> morseList;
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
    explicit MessageListPage(PageStack& stack);
};

#endif //MORSEMACHINE_MESSAGELISTPAGE_H