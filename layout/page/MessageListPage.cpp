#include "MessageListPage.h"

#include "GlobalState.h"
#include "MorseListenPage.h"
#include "layout/net/RequestUtil.h"

MessageListPage::MessageListPage(PageStack &stack) : OptionPage(stack) { }

std::string MessageListPage::getTitle() {
    return "== total: " + std::to_string(morseList.size()) + " ==";
}

void MessageListPage::initPage() {
    RequestUtil::getMorseList(3, morseList);
}


void MessageListPage::getAllOptions(std::vector<Option> &allOption, const size_t height) {
    // 选项就是morse数据
    // *用户名     月-日
    const size_t pageSize = height - 1;
    const uint32_t start = (page - 1) * pageSize;

    std::string str;
    for (uint32_t i = start; i < start + std::min(morseList.size(), pageSize); i++) {
        str.clear();

        // 是否已读
        const VSimpleMorse& morse = morseList[i];
        if (morse.state == 1) str += "*";

        // 用户名如果超过三位则超过的显示..
        if (morse.sender.userName.size() > 3) {
            str += morse.sender.userName.substr(0, 3) + "..";
        } else {
            str += morse.sender.userName;
        }

        // 截取时间
        const std::string morseDay = morse.createTime.substr(5, 5);
        str += "  ";
        str += morseDay;

        allOption.emplace_back(Option{i, 0, str});
    }

    // 是不是最后一页
    if (allOption.size() != height - 1) isLastPage = true;
}

bool MessageListPage::selectOption(Option &option) {
    // 找到选中的 存入id 跳转页面
    VSimpleMorse& morse = morseList[option.id];
    GlobalState::MORSE_LISTEN_MORSE_ID = morse.morseId;
    morse.state = 2;

    Page* newPage = new MorseListenPage(stack);
    stack.pushPage(*newPage);

    return false;
}

bool MessageListPage::userInput00(std::string key) {
    // 这个页面没什么其他可输入的
    return false;
}


void MessageListPage::onWrapAround() {
    if (isLastPage) return;

    page++;
}

