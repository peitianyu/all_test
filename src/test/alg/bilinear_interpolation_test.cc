#include "core/tt_test.h"
#include "common/log.h"
#include <iostream>
#include <Eigen/Core>

float get_value(float x, float y, const Eigen::MatrixXd& data)
{
    int width = data.cols();
    int height = data.rows();

    if(x < 0) x = 0;
    if(x > width - 1) x = width - 1;
    if(y < 0) y = 0;
    if(y > height - 1) y = height - 1;

    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float v00 = data(y0, x0);
    float v01 = data(y0, x1);
    float v10 = data(y1, x0);
    float v11 = data(y1, x1);

    float v0 = (x1 - x) * v00 + (x - x0) * v01;
    float v1 = (x1 - x) * v10 + (x - x0) * v11;

    return (y1 - y) * v0 + (y - y0) * v1;
}

// JUST_RUN_TEST(bilinear_interpolation, test)
TEST(bilinear_interpolation, test)
{
    LOG_TEST("------------------------------------bilinear interpolation test-------------------------------------");

    Eigen::Matrix4d data;
    data << 1, 2, 3, 4,
            2, 3, 4, 5,
            3, 4, 5, 6,
            4, 5, 6, 7;
    
    float v = get_value(1.5, 4.5, data);

    LOG_TEST("[1.5, 1.5]: ", v);
}
