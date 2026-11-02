#ifndef MORSEMACHINE_SENDMORSEPAGE_H
#define MORSEMACHINE_SENDMORSEPAGE_H
#include "GlobalState.h"
#include "PageConstant.h"
#include "layout/HintOptionsPage.h"

class SendMorsePage : public HintOptionsPage {
    // 发送的用户
    VSimpleUser acceptUser = {0, ""};
    // 电文 (二进制)
    std::vector<bool>* code = GlobalState::INPUT_CODE_PAGE_VECTOR;
    // 在其他页面输入的数据名称
    std::string otherPageBackInputName;

    std::string getTitle() override;

    void getAllOptions(std::vector<Option> &allOption, size_t height) override;

    bool selectOption(Option &option) override;

    bool userInput00(std::string key) override;

    void showPage() override;

    // 把二进制电文转为16进制格式的字符串
    std::string parseCodeTo16() const;

public:
    explicit SendMorsePage(PageStack& stack);

    ~SendMorsePage() override;
};


#endif //MORSEMACHINE_SENDMORSEPAGE_H