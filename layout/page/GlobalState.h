#ifndef MORSEMACHINE_GLOBALSTATE_H
#define MORSEMACHINE_GLOBALSTATE_H

#include <string>
#include <cstdint>
#include <vector>

class GlobalState {
public:
    // 用户登录token
    static inline std::string TOKEN;

    // Input页面的输入内容
    static inline std::string INPUT_PAGE_TEXT;
    // Input页面输入的最大长度
    static inline uint8_t INPUT_PAGE_TEXT_MAX_SIZE = 10;

    // InputCode页面输入的内容
    static inline auto INPUT_CODE_PAGE_VECTOR = new std::vector<bool>();

    // MorseListen页面的morseId
    static inline uint32_t MORSE_LISTEN_MORSE_ID;

    // WifiPage页面的wifi SSID
    static inline std::string WIFI_SELECTED_SSID;

};

#endif //MORSEMACHINE_GLOBALSTATE_H