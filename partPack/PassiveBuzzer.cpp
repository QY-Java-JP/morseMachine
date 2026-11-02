#include "PassiveBuzzer.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

PassiveBuzzer::PassiveBuzzer(const uint8_t pin) {
    // 初始化
    this->pin = pin;
    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice = pwm_gpio_to_slice_num(pin);

    // 设置频率
    pwm_set_clkdiv(slice, 125.0f);
}

// 响一声
void PassiveBuzzer::beep(const uint freq, const uint ms) const {
    pwm_set_wrap(slice, 125000000 / freq / 125);
    pwm_set_gpio_level(pin,500);

    pwm_set_enabled(slice, true);
    sleep_ms(ms);
    pwm_set_enabled(slice, false);
}

// 音符
void PassiveBuzzer::beep(SoundItem& item) const {
    beep(static_cast<uint>(item.note), item.ms);
}


