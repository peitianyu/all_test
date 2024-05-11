#include "core/tt_test.h"
#include "common/threadsafe_queue.h"

#include <thread>
#include <iostream>

// 测试函数
void test_threadsafe_queue()
{
    threadsafe_queue<int> tsq;

    // 创建一个生产者线程
    std::thread producer([&]() {
    for (int i = 0; i < 10; ++i)
    {
    std::cout << "Pushing " << i << std::endl;
    tsq.push(i);
    } });

    // 创建两个消费者线程
    std::thread consumer1([&]() {
    for (int i = 0; i < 5; ++i)
    {
    int value;
    tsq.wait_and_pop(value);
    std::cout << "Consumer 1 popped " << value << std::endl;
    } });

    std::thread consumer2([&]() {
    for (int i = 0; i < 5; ++i)
    {
    int value;
    tsq.wait_and_pop(value);
    std::cout << "Consumer 2 popped " << value << std::endl;
    } });

    // 等待所有线程完成
    producer.join();
    consumer1.join();
    consumer2.join();
}

// JUST_RUN_TEST(threadsafe_queue, test)
TEST(threadsafe_queue, test)
{
    test_threadsafe_queue();
}