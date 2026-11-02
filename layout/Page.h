#ifndef MYPROJECT_PAGE_H
#define MYPROJECT_PAGE_H

#include <string>
#include <vector>

#include "PageStack.h"

class PageStack;

// 一个页面布局
struct PageData {
    // 多少行
    int maxHeight;
    // 一行多宽
    int maxWidth;
    // 每行的内容
    std::vector<std::string> lines;
};

// 一个页面
class Page {
    // 是否可以返回
    bool canBack = true;

public:
    // next按下
    static inline const std::string NEXT_BTN_PUSH = "_next_";
    // OK按下
    static inline const std::string OK_BTN_PUSH = "_ok_";
    //back按下
    static inline const std::string BACK_BTN_PUSH = "_back_";

protected:
    // 页面栈
    PageStack& stack;

    // 修改是否可以返回
    void setCanBack(const bool can) {
        canBack = can;
    }

public:
    explicit Page(PageStack& pageStack) : stack(pageStack) {}

    virtual ~Page() = default;

    // 初始化页面
    virtual void initPage() {}

    // 当页面展示
    virtual void showPage() {}

    // 当页面隐藏
    virtual void hidePage() {}

    // 当页面刷新
    virtual void pageFlush() {}

    // 展示页面
    virtual void printPage(PageData& page) = 0;

    // 用户输入指令
    virtual bool userInput0(std::string key) = 0;

    // 用户输入指令
    bool userInput(const std::string& key) {
        // 看看是不是back
        if (key == BACK_BTN_PUSH) {
            if (canBack) stack.back();

            return false;
        }

        return userInput0(key);
    }
};

#endif //MYPROJECT_PAGE_H