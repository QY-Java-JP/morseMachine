#ifndef MORSEMACHINE_INPUCODEPAGE_H
#define MORSEMACHINE_INPUCODEPAGE_H

#include "GlobalState.h"
#include "layout/Page.h"
#include "partPack/Buzzer.h"

class InputCodePage : public Page {
    // 二进制数据
    std::vector<bool>* code = GlobalState::INPUT_CODE_PAGE_VECTOR;
    // 蜂鸣器
    Buzzer* buzzer;
    // 刷新次数
    uint8_t flushCount = 0;

    void initPage() override;

    void pageFlush() override;

    void printPage(PageData &page) override;

    bool userInput0(std::string key) override;

public:
    // 刷新多少次记录一次
    static constexpr uint8_t WRITE_CODE_FLUSH_COUNT_DET = 5;
    // 记录时间间隔
    static constexpr long WRITE_CODE_DET_MS = WRITE_CODE_FLUSH_COUNT_DET * PageStack::CHECK_INPUT_MS;

    explicit InputCodePage(PageStack& stack);
};


#endif //MORSEMACHINE_INPUCODEPAGE_H