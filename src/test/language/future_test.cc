#include"core/tt_test.h"

#include <iostream>
#include <future>
#include <vector>

JUST_RUN_TEST(future, test)
TEST(future, test)
{
    // 使用future实现0~1000求和 
    std::vector<std::future<int>> futures;
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(std::async(std::launch::async, [i] {
            int result = 0;
            for (int j = 0; j < 100; ++j) {
                result += i * 100 + j;
            }
            return result;
        }));
    }

    for (auto &future : futures) {
        sum += future.get();
    }

    std::cout << "sum: " << sum << std::endl;
}   

