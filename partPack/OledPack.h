#ifndef MYPROJECT_OLEDPACK_H
#define MYPROJECT_OLEDPACK_H

#include <string>

#include "ssd1306.h"

class OledPack {
    // 默认字体放大倍数
    const int defaultScale = 1;
    // 高度
    int _height = 0;
    // 宽度
    int _width = 0;

    // 屏幕
    ssd1306_t disp = {};

public:
    OledPack(int sda, int scl, int height, int width);

    // 显示字符串
    void showString(const std::string &str, int line);

    // 清屏
    void clean();

    [[nodiscard]] int getHeight() const {
        return _height;
    }

    [[nodiscard]] int getWidth() const {
        return _width;
    }
};


#endif //MYPROJECT_OLEDPACK_H