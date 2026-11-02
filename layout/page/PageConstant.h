#ifndef MORSEMACHINE_PAGECONSTANT_H
#define MORSEMACHINE_PAGECONSTANT_H
#include <map>
#include <string>
#include <cstdint>

struct VSimpleUser {
    uint64_t userId;
    std::string userName;
};

struct VSimpleMorse {
    uint32_t morseId{};
    VSimpleUser sender;
    uint8_t state{};
    std::string createTime;
};

class PageConstant {
public:
    // 当电报键按下
    static inline const std::string MORSE_PUSH = "morsePush";
    // 当电报键弹起
    static inline const std::string MORSE_UP = "morseUp";

    // 有源蜂鸣器的key
    static inline const std::string BUZZER_KEY = "buzzerKey";

    // 摩斯电码表
    static inline const std::map<std::string, char> MORSE_TABLE = {
        // 英文字母
        {"01", 'A'},
        {"1000", 'B'},
        {"1010", 'C'},
        {"100", 'D'},
        {"0", 'E'},
        {"0010", 'F'},
        {"110", 'G'},
        {"0000", 'H'},
        {"00", 'I'},
        {"0111", 'J'},
        {"101", 'K'},
        {"0100", 'L'},
        {"11", 'M'},
        {"10", 'N'},
        {"111", 'O'},
        {"0110", 'P'},
        {"1101", 'Q'},
        {"010", 'R'},
        {"000", 'S'},
        {"1", 'T'},
        {"001", 'U'},
        {"0001", 'V'},
        {"011", 'W'},
        {"1001", 'X'},
        {"1011", 'Y'},
        {"1100", 'Z'},

        // 数字
        {"01111", '1'},
        {"00111", '2'},
        {"00011", '3'},
        {"00001", '4'},
        {"00000", '5'},
        {"10000", '6'},
        {"11000", '7'},
        {"11100", '8'},
        {"11110", '9'},
        {"11111", '0'}
    };

};

#endif //MORSEMACHINE_PAGECONSTANT_H