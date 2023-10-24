#include "core/tt_test.h"
#include "common/log.h"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Core>


// JUST_RUN_TEST(cam_pos, base)
TEST(cam_pos, base)
{
    // float fx = 0.718856, fy = 0.718856, cx = 0.6071928, cy = 0.1852157, bl = 0.386145;
    Eigen::Matrix3f K;
    K << 0.718856, 0, 0.6071928,
         0, 0.718856, 0.1852157,
         0, 0, 1;
    
    Eigen::Matrix3f R;
    R << 0.9999, 0.0106, -0.0095,
         -0.0106, 0.9999, -0.0095,
         0.0095, 0.0095, 0.9999;
    Eigen::Vector3f t(0.0469029, 0.0283993, -0.858694);
    float scale = 10.0;

    Eigen::Vector3f ext_r = Eigen::Vector3f::Zero(); // alpha, beta, gamma
    Eigen::Quaternionf ext_q(ext_r(0), ext_r(1), ext_r(2), 1);
    Eigen::Matrix3f ext_R = ext_q.toRotationMatrix();
    float baseline = 0.386145;

    Eigen::Vector3f p0(50, 50, 1); // 齐次相机坐标系下的点(u, v, 1)

    // (R[01], t[01]) = [R, scale*t] * [ext_R, b]  =>   T01
    //                  [0,       1]   [0,     1]

    // 求解T01
    Eigen::Matrix4f T01 = Eigen::Matrix4f::Identity();
    T01 << R(0, 0), R(0, 1), R(0, 2), scale*t(0),
           R(1, 0), R(1, 1), R(1, 2), scale*t(1),
           R(2, 0), R(2, 1), R(2, 2), scale*t(2),
           0, 0, 0, 1;

    Eigen::Matrix4f ext_T = Eigen::Matrix4f::Identity();
    ext_T.block<3, 3>(0, 0) = ext_R;
    ext_T(0, 3) = baseline;

    T01 = T01 * ext_T;
    LOG_TEST("T01: \n", T01);

    // 求解本质矩阵
    // 反对称矩阵
    Eigen::Vector3f t1 = T01.block<3, 1>(0, 3);
    Eigen::Matrix3f t_hat;
    t_hat << 0, -t1(2), t1(1),
             t1(2), 0, -t1(0),
             -t1(1), t1(0), 0;
    
    Eigen::Matrix3f E = t_hat * R;
    LOG_TEST("E: \n", E);

    // 求极线
    Eigen::Vector3f line = E * p0;
    LOG_TEST("line: ", line.transpose());

    // 求点到直线距离
    Eigen::Vector3f p1(50, 50, 1);
    float d = std::abs(p1.dot(line) / line.norm());
    LOG_TEST("d: ", d);

    // d 为残差, ext_r(α, β, γ)与scale为待优化变量, 构造雅各比
    Eigen::Matrix<float, 1, 4> J;
     
}