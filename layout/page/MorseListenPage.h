#ifndef MORSEMACHINE_MORSELISTENPAGE_H
#define MORSEMACHINE_MORSELISTENPAGE_H
#include "layout/Page.h"
#include "partPack/Buzzer.h"

class MorseListenPage : public Page {
    // 电文
    std::vector<bool> morse;
    // 目前页面停留时长
    long showMs = 0;
    // 是否播放完毕
    bool playOver = false;

    // 蜂鸣器
    Buzzer* buzzer;

    void initPage() override;

    void pageFlush() override;

    bool userInput0(std::string key) override;

    void printPage(PageData &page) override;

    // 把16进制字符串解码为二进制
    void parseCode16(const std::string& code);

public:
    MorseListenPage(PageStack& stack);
};


#endif //MORSEMACHINE_MORSELISTENPAGE_H