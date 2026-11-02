#ifndef MORSEMACHINE_MENUPAGE_H
#define MORSEMACHINE_MENUPAGE_H
#include <layout/Page.h>

#include "layout/OptionPage.h"

class MenuPage : public OptionPage {
public:
    explicit MenuPage(PageStack& page_stack);

    // 获取列表
    void getAllOptions(std::vector<Option>& allOption, size_t height) override;

    // 用户选择了选项
    bool selectOption(Option& option) override;

    // 用户输入指令
    bool userInput00(std::string key) override;

    // 标题
    std::string getTitle() override;
};


#endif //MORSEMACHINE_MENUPAGE_H