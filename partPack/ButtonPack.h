#ifndef MYPROJECT_BUTTONPACK_H
#define MYPROJECT_BUTTONPACK_H

#include <cstdint>

#include "map"
#include "string"

class ButtonPack {
    // 一次判断的时间ms
    static constexpr int INTERVAL_MS = 50;

    // 按钮pid
    uint8_t pid = 0;
    // 当前按下的时间
    int clickedMs = 0;
    // 附加属性
    std::map<std::string, void*> attr;
    // 单机事件回调
    void (*onPress)(ButtonPack&) = nullptr;
    // 长按事件回调
    void (*onLongPress)(ButtonPack&) = nullptr;
    // 回调事件是否绑定完毕
    bool callbackInited = false;

    // 初始化
    void initCallBack();

    // 执行的方法
    void run();

public:
    explicit ButtonPack(uint8_t pid);

    // 绑定点击回调
    void registerOnPress(void(*callback)(ButtonPack&));
    // 绑定长按回调
    void registerOnLongPass(void(*callback)(ButtonPack&));

    // 添加参数
    template<typename T>
    void addAttr(const std::string& key, T* value) {
        attr[key] = static_cast<void*>(value);
    }

    // 删除参数
    void deleteAttr(const std::string& key) {
        attr.erase(key);
    }

    // 获取参数
    template<typename T>
    T* getAttr(const std::string &key) {
        return static_cast<T*>(attr[key]);
    }
};


#endif //MYPROJECT_BUTTONPACK_H