#ifndef MYPROJECT_PAGESTACK_H
#define MYPROJECT_PAGESTACK_H

#include <map>
#include <stack>
#include <partPack/OledPack.h>

class Page;

#define PAGE_STACK_UN_INPUT "_&UNI_"

class PageStack {
    // 页面栈
    std::stack<Page*> pageStack;
    // OLED
    OledPack& oled;

    // 用户输入的内容
    std::string input;
    // 是否需要刷新页面
    bool needPlush = false;

    // run
    void run();

    // 显示一个页面
    void showOnePage(Page* page);

public:
    // 检查一次用户输入的时间ms
    static constexpr long CHECK_INPUT_MS = 10;

    // 共享数据
    std::map<std::string, void*> shareDataMap;

    explicit PageStack(OledPack& oledPack);

    // 启动时压入初始页面
    void start(Page& indexPage);

    // 压入一个页面
    void pushPage(Page& page);

    // 替换一个页面
    void replaceTopPage(Page& page);

    // 弹出其他页面压入一个页面
    void resetTo(Page& page);

    // 弹出栈顶页面
    void back();

    // 设置input
    void setInput(std::string key);

    // 手动刷新当前页面
    void flushTopPage();
};


#endif //MYPROJECT_PAGESTACK_H