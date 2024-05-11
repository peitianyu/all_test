#ifndef __SIMPLE_MUTEX_H__
#define __SIMPLE_MUTEX_H__

#include <atomic>
#include <thread>


class SimpleMutex
{
public:
    void lock() {
        while(m_flag.test_and_set(std::memory_order_acquire));
    }

    void unlock() {
        m_flag.clear(std::memory_order_release);
    }

    bool try_lock() {
        return !m_flag.test_and_set(std::memory_order_acquire);
    }
private:
    std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

class simple_mutex {
public:
    void lock() {
        while (flag.exchange(true, std::memory_order_acquire)) {
            // 自旋等待锁的释放
            std::this_thread::yield();
        }
    }

    void unlock() {
        flag.store(false, std::memory_order_release);
    }

    bool try_lock() {
        return flag.fetch_add(1, std::memory_order_acquire) == 0;
    }

private:
    std::atomic<int> flag{0};
};


template <typename MUTEX>
class LockGuard
{
public:
    LockGuard(MUTEX& mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }

    ~LockGuard() {
        m_mutex.unlock();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
private:
    MUTEX& m_mutex;
};

#endif // __SIMPLE_MUTEX_H__