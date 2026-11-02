#ifndef MORSEMACHINE_BUZZER_H
#define MORSEMACHINE_BUZZER_H
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/types.h"

class Buzzer {
    uint pin;
    bool _isOn = false;

public:
    explicit Buzzer(uint gpioPin) : pin(gpioPin) {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, false);
    }

    void on() {
        _isOn = true;
        gpio_put(pin, true);
    }

    void off() {
        _isOn = false;
        gpio_put(pin, false);
    }

    void beep(uint32_t durationMs) {
        on();
        sleep_ms(durationMs);
        off();
    }

    [[nodiscard]] bool isOn() const {
        return _isOn;
    }
};

#endif //MORSEMACHINE_BUZZER_H