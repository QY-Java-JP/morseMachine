#ifndef MORSEMACHINE_REGISTERPAGE_H
#define MORSEMACHINE_REGISTERPAGE_H
#include "layout/HintOptionsPage.h"
#include "layout/OptionPage.h"

class RegisterPage : public HintOptionsPage {
    // 用户名
    std::string uname;
    // 密码
    std::string pwd;

    // 进入input页面时候输入的东西
    std::string inputPageInputDataName = "uname";

    void getAllOptions(std::vector<Option> &allOption, size_t height) override;

    bool selectOption(Option &option) override;

    bool userInput00(std::string key) override;

    void showPage() override;

    std::string getTitle() override;

public:
    // 用户名最大长度
    static inline uint8_t UNAME_MAX_SIZE = 10;
    // 密码最大长度
    static inline uint8_t UPWD_MAX_SIZE = 20;

    explicit RegisterPage(PageStack& stack);
};

#endif //MORSEMACHINE_REGISTERPAGE_H