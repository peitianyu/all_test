#include "core/tt_test.h"

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

std::queue<int> dataQueue;  // 数据队列
std::mutex mtx;             // 互斥锁，保护数据队列
std::condition_variable cv; // 条件变量，用于线程通信
const int MaxSize = 10;     // 数据队列的最大大小

// 生产者函数
void producer()
{
    for (int i = 1; i <= 20; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 模拟生产耗时

        std::unique_lock<std::mutex> lock(mtx); // 加锁
        cv.wait(lock, []
                { return dataQueue.size() < MaxSize; }); // 队列满时等待条件变量
        dataQueue.push(i);                               // 生产数据
        std::cout << "Producer produced data: " << i << std::endl;
        cv.notify_all(); // 唤醒等待的消费者线程
    }
}

// 消费者函数
void consumer()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 模拟消费耗时

        std::unique_lock<std::mutex> lock(mtx); // 加锁
        cv.wait(lock, []
                { return !dataQueue.empty(); }); // 队列空时等待条件变量
        int data = dataQueue.front();            // 消费数据
        dataQueue.pop();
        std::cout << "Consumer consumed data: " << data << std::endl;
        cv.notify_all(); // 唤醒等待的生产者线程

        if (data == 20) // 终止条件
            break;
    }
}

// JUST_RUN_TEST(producer_consumer, test)
TEST(producer_consumer, test)
{
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
}
