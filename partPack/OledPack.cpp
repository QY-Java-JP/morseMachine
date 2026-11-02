#include "OledPack.h"

OledPack::OledPack(const int sda, const int scl, const int height, const int width) {
    // 初始化i2c
    i2c_init(i2c0, 400000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    _height = height;
    _width = width;

    // 初始化屏幕
    ssd1306_init(&disp, width * 8, height * 8, 0x3C, i2c0);
    clean();
}

// 显示一行字符
void OledPack::showString(const std::string &str, const int line) {
    // 画 (x y 字体倍数 字)
    const int y = defaultScale * 8 * (line - 1);
    ssd1306_draw_string(&disp, 0, y, defaultScale, str.c_str());

    // 显示
    ssd1306_show(&disp);
}

// 清屏
void OledPack::clean() {
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}


