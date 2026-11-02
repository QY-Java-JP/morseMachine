#ifndef MORSEMACHINE_OPTIONPAGE_H
#define MORSEMACHINE_OPTIONPAGE_H
#include <cstdint>

#include "Page.h"

// 选项
struct Option {
    // id
    uint32_t id = 0;
    // 间隔
    uint8_t marginTop;
    // 文字描述
    std::string text;
};

class OptionPage : public Page {
    // 选中的选项下标
    int selectOptionIndex = 0;
    // 所有选项
    std::vector<Option> options;

protected:
    // 获取所有选项
    virtual void getAllOptions(std::vector<Option>& allOption, size_t height) = 0;

    // 用户选择了什么选项
    virtual bool selectOption(Option& option) = 0;

    // 用户输入非down和push的指令 也就是不是这个类要拦截的指令
    virtual bool userInput00(std::string key) = 0;

    // 当选项回归第一条
    virtual void onWrapAround() {}

    // 标题
    virtual std::string getTitle() {
        return "";
    }

public:
    explicit OptionPage(PageStack& stack) : Page(stack) {}

    // 打印页面
    void printPage(PageData &page) override {
        // 首先修改选中的选项 然后打印所有选项
        options.clear();
        getAllOptions(options, page.maxHeight);

        // title
        const std::string title = getTitle();
        if (!title.empty()) {
            page.lines[0] = title;
        }

        // 选项
        for (int i = 0; i < options.size(); i++) {
            // 注意margin
            Option& option = options[i];
            option.text.insert(0, i == selectOptionIndex ? "> " : "  ");
            page.lines[title.empty() ? i+option.marginTop : i+option.marginTop+1] = options[i].text;
        }
    }

    // 用户输入
    bool userInput0(const std::string key) override {
        // 看看输入的是不是 OK 或 NEXT
        if (key == OK_BTN_PUSH) {
            // 那么用户选择这个页面
            const std::string subStr = options[selectOptionIndex].text.substr(2);
            options[selectOptionIndex].text = subStr;
            return selectOption(options[selectOptionIndex]);
        }

        if (key == NEXT_BTN_PUSH) {
            // 往下一个
            if (selectOptionIndex >= options.size() - 1) {
                onWrapAround();
                selectOptionIndex = 0;
            } else {
                ++selectOptionIndex;
            }

            return true;
        }

        return userInput00(key);
    }
};

#endif //MORSEMACHINE_OPTIONPAGE_H