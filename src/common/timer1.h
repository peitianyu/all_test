#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>

class Timer {
public:
    using Ptr = std::shared_ptr<Timer>;
    using Callback = std::function<void()>;

public:
    Timer(Callback callback, int ms) : should_quit_(false), callback_(callback), interval_ms_(ms) 
    {
        thread_ = std::thread(&Timer::Run, this);
    }

    static Ptr Create(Callback callback, int ms) 
    {
        return std::make_shared<Timer>(callback, ms);
    }

    void Quit() 
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            should_quit_ = true;
        }
        cv_.notify_one();
    }

    ~Timer() 
    {
        Quit();
        if (thread_.joinable()) thread_.join();
    }

private:
    void Run() 
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!should_quit_) {
            cv_.wait_for(lock, std::chrono::milliseconds(interval_ms_), [this]{ return should_quit_; });
            if (should_quit_) break;

            callback_(); 
        }
    }
protected:
    std::thread thread_;
    bool should_quit_;
    std::mutex _mutex;
    std::condition_variable cv_;
    Callback callback_;
    int interval_ms_;
};

#endif // __TIMER_H__