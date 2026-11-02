#ifndef MYPROJECT_BREATHINGLIGHT_H
#define MYPROJECT_BREATHINGLIGHT_H
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/time.h>
#include <sys/types.h>
#include <cmath>

class BreathingLight {
    // 默认呼吸时长
    inline static uint DEFAULT_BREATHING_TIME = 128 * 2 * 10;

    // pin
    uint8_t pin = 0;

public:
    explicit BreathingLight(const uint8_t pin) {
        this->pin = pin;
        gpio_set_function(pin, GPIO_FUNC_PWM);
        const uint slice = pwm_gpio_to_slice_num(pin);
        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 4.0f);
        pwm_init(slice, &config, true);
        pwm_set_gpio_level(pin, 0);
    }

    // 呼吸
    void breathing(const uint ms) const {
        // 亮灭各占一半
        const uint time = std::max(ms / 256, 1u);
        for (int i = 0; i <= 255; i++) {
            const float x = (1 - cosf(i * M_PI / 255)) / 2;
            pwm_set_gpio_level(pin, x * 65535);
            sleep_ms(time);
        }
        for (int i = 254; i >= 0; i--) {
            const float x = (1 - cosf(i * M_PI / 255)) / 2;
            pwm_set_gpio_level(pin, x * 65535);
            sleep_ms(time);
        }
    }

    // 呼吸
    void breathing() const {
        breathing(DEFAULT_BREATHING_TIME);
    }
};

#endif //MYPROJECT_BREATHINGLIGHT_H