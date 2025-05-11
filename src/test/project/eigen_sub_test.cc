#include "core/tt_test.h"
#include "common/log.h"
#include <iostream>
#include <Eigen/Dense>

// JUST_RUN_TEST(eigen_sub, test);
TEST(eigen_sub, test)
{
    // 定义两个4x4的uint8_t矩阵
    Eigen::Matrix<uint8_t, 4, 4> matrix1;
    Eigen::Matrix<uint8_t, 4, 4> matrix2;

    // 初始化矩阵
    matrix1 << 100, 200, 150, 50,
               200, 100, 50, 200,
               50, 200, 100, 150,
               150, 50, 200, 100;
    matrix2 << 255, 100, 200, 100,
               100, 255, 100, 255,
               200, 100, 255, 50,
               50, 255, 100, 255;

    // 计算绝对值的和
    uint sum = (matrix1.cast<int>() - matrix2.cast<int>()).cwiseAbs().sum();

    std::cout << "Sum of absolute differences: " << sum << std::endl;
}