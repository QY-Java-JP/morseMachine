#ifndef MYPROJECT_PASSIVEBUZZER_H
#define MYPROJECT_PASSIVEBUZZER_H
#include <pico/time.h>
#include <sys/types.h>

// 音符
enum class MusicNote : uint {
    DO = 262,
    RE = 294,
    MI = 330,
    FA = 349,
    SOL = 392,
    LA = 440,
    SI = 494
};

// 声音单位
struct SoundItem {
    MusicNote note;
    uint ms;
};

// 无源蜂鸣器
class PassiveBuzzer {
    // pin
    uint8_t pin = 0;
    // slice
    uint slice = 0;

public:
    explicit PassiveBuzzer(uint8_t pin);

    // 响一声
    void beep(uint freq, uint ms) const;

    // 响一声(音符)
    void beep(SoundItem& item) const;

    // 弹奏一曲
    // 弹奏
    template<size_t N>
    void music(SoundItem (&items)[N]) {
        for (int i = 0; i < N; i++) {
            beep(items[i]);
            // 结束一个停顿一段时间
            sleep_ms(200);
        }
    }

};

#endif
