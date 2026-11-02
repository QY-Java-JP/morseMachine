#ifndef MORSEMACHINE_INPUTPAGE_H
#define MORSEMACHINE_INPUTPAGE_H

#include <cstdint>
#include <layout/Page.h>

#include "partPack/Buzzer.h"

class InputPage : public Page {
    // 输入的字符
    std::string inputString;
    // 当前输入的电码
    std::string inputMorse;
    // 电码最大长度
    uint8_t maxSize = 10;
    // 蜂鸣器
    Buzzer* buzzer;

    // 按下的时间
    uint32_t intervalMs = 0;
    // 这次按下的结果是否处理过
    bool pushDataHandled = true;

    // 解码当前输入的电报码
    void parseInputMorse();

public:
    explicit InputPage(PageStack& page_stack);

    void initPage() override;

    void pageFlush() override;

    void hidePage() override;

    // 展示页面
    void printPage(PageData& page) override;

    // 用户输入指令
    bool userInput0(std::string key) override;
};


#endif //MORSEMACHINE_INPUTPAGE_H