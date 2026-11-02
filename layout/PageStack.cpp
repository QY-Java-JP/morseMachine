#include "PageStack.h"

#include <TaskScheduler.hpp>

#include "Page.h"

PageStack::PageStack(OledPack &oledPack) : oled(oledPack) {
    oledPack.clean();
}

void PageStack::start(Page &indexPage) {
    // 压入初始页面
    pushPage(indexPage);
    TaskScheduler::setInterval([this] {run();}, CHECK_INPUT_MS);
}


// 压入一个页面
void PageStack::pushPage(Page &page) {
    // 调用当前页面的隐藏方法
    if (!pageStack.empty()) {
        Page* topPage = pageStack.top();
        topPage->hidePage();
    }

    // 初始化展示这个页面
    page.initPage();
    page.showPage();
    pageStack.push(&page);
    needPlush = true;
}

// 替换页面
void PageStack::replaceTopPage(Page &page) {
    // 销毁当前页面 压入新的页面
    Page* topPage = pageStack.top();
    pageStack.pop();
    topPage->hidePage();
    delete topPage;

    page.initPage();
    page.showPage();
    pageStack.push(&page);
    needPlush = true;
}

// 清空页面栈 压入一个新的页面
void PageStack::resetTo(Page &page) {
    // 清空所有页面
    while (!pageStack.empty()) {
        Page* topPage = pageStack.top();
        pageStack.pop();
        topPage->hidePage();
        delete topPage;
    }

    // 压入新的页面
    pushPage(page);
}


// 返回页面
void PageStack::back() {
    // 弹出这个页面
    Page* topPage = pageStack.top();
    pageStack.pop();
    topPage->hidePage();
    if (!pageStack.empty()) {
        pageStack.top()->showPage();
    }

    needPlush = true;
    delete topPage;

    // 如果没页面了终止程序
    if (pageStack.empty()) std::exit(0);
}

// run
void PageStack::run() {
    Page* topPage = pageStack.top();
    topPage->pageFlush();

    // 由于 pageFlush中可能调用back()等销毁此页面 所以这里要再获取一次栈顶元素
    topPage = pageStack.top();

    // 是否需要刷新页面
    if (needPlush) showOnePage(topPage);

    // 检查用户是否输入了
    if (input != PAGE_STACK_UN_INPUT) {
        // 调用输入
        if (topPage->userInput(input)) {
           needPlush = true;
        }

        input = PAGE_STACK_UN_INPUT;
    }
}

// 显示一个页面
void PageStack::showOnePage(Page* page) {
    // 构建一个结构 先交给页面编辑
    needPlush = false;
    PageData data = {
        oled.getHeight(),
        oled.getWidth(),
        {static_cast<std::vector<std::string>::size_type>(oled.getHeight()), ""}
    };

    page->printPage(data);
    oled.clean();

    // 一行行的输出
    const int maxLine = data.maxHeight > data.lines.size() ? data.lines.size() : data.maxHeight;
    for (int i = 0; i < maxLine; i++) {
        oled.showString(data.lines[i], i + 1);
    }
}

void PageStack::setInput(const std::string key) {
    input = key;
}

// 手动刷新当前页面
void PageStack::flushTopPage() {
    needPlush = true;
}

