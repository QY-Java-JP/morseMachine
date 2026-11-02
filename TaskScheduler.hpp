#ifndef MYPROJECT_TASKSCHEDULER_H
#define MYPROJECT_TASKSCHEDULER_H

#include <vector>
#include "pico/time.h"
#include "pico/multicore.h"
#include "functional"
#include "util/Logger.h"

class TaskScheduler {
    struct Task {
        // id
        uint32_t id;
        // 时间间隔
        uint32_t interval;
        // 下次执行的时间
        uint32_t nextRunMs;
        // 是否继续执行
        bool multExec;
        // 任务
        std::function<void()> callback;
    };

    // 是否初始化过了
    inline static bool inited = false;
    // 任务集合
    inline static std::vector<Task> taskArray;
    // 唯一id
    inline static uint32_t nextId = 0;

    // 是否为异步模式
    inline static bool isAsync = true;

    // 遍历执行任务
    static void run() {
        // 每隔20ms检查一次
        for (;;) {
            coreRun();
            sleep_ms(LOOP_WAIT_MS);
        }
    }

    // 核心逻辑
    static void coreRun() {
        const uint32_t now = to_ms_since_boot(get_absolute_time());
        for (auto t = taskArray.begin(); t != taskArray.end();) {
            if (t->nextRunMs > now) {
                ++t;
                continue;
            }

            // 执行
            t->callback();
            // 看看下次是否还需要执行
            if (t->multExec) {
                t->nextRunMs += t->interval;
                ++t;
            } else {
                t = taskArray.erase(t);
            }
        }
    }

public:
    // 轮询时间
    static inline uint32_t LOOP_WAIT_MS = 20;

    // 初始化
    static void init(bool async = true) {
        if (inited) return;

        // 开线程
        inited = true;
        isAsync = async;

        // 如果是异步模式 我们用一个线程 如果是同步模式 不用管
        if (isAsync) {
            multicore_launch_core1(run);
            Logger::info("TaskScheduler", "Task Scheduler 异步方式启动");
        }
    }

    // 同步模式轮询任务
    static void syncRun() {
        if (isAsync) return;

        coreRun();
    }

    // 添加任务
    template<class F>
    static void setTimeOut(const F callback, const uint32_t interval) {
        // 加进去即可
        const uint32_t now = to_ms_since_boot(get_absolute_time());
        const Task newTask = {nextId++, interval, now + interval, false, callback};
        taskArray.push_back(newTask);
        Logger::info("TaskScheduler", "新建延时任务: " + std::to_string(newTask.id));
    }

    // 添加任务
    template<class F>
    static void setInterval(const F callback, const uint32_t interval) {
        // 加进去即可
        const uint32_t now = to_ms_since_boot(get_absolute_time());
        const Task newTask = {nextId++, interval, now + interval, true, callback};
        taskArray.push_back(newTask);
        Logger::info("TaskScheduler", "新建循环任务: " + std::to_string(newTask.id));
    }
};


#endif //MYPROJECT_TASKSCHEDULER_H