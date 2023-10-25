#include "core/tt_test.h"
#include "common/log.h"
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Core>

Eigen::Matrix3f hat(const Eigen::Vector3f& v);

// JUST_RUN_TEST(cam_pos, base)
TEST(cam_pos, base)
{
     Eigen::Matrix3f R;
     R << 0.999989, -0.00277916, -0.00382672,
         0.00277142, 0.999994, -0.00202726,
         0.00383234, 0.00201664, 0.999991;
     Eigen::Vector3f t(0.0190076, -0.0133997, 0.99973);
     float scale = 0.68;

    Eigen::Vector3f ext_r = Eigen::Vector3f::Zero(); // alpha, beta, gamma
    Eigen::Quaternionf ext_q(1, ext_r(0), ext_r(1), ext_r(2));
    Eigen::Matrix3f ext_R = ext_q.toRotationMatrix();
    float baseline = 0.386145;

    Eigen::Matrix4f T01 = Eigen::Matrix4f::Identity();
    T01 << R(0, 0), R(0, 1), R(0, 2), t(0)*scale,
           R(1, 0), R(1, 1), R(1, 2), t(1)*scale,
           R(2, 0), R(2, 1), R(2, 2), t(2)*scale,
           0, 0, 0, 1;

    Eigen::Matrix3f K;
    K << 718.856, 0, 607.1928,
         0, 718.856, 185.2157,
         0, 0, 1;

    Eigen::Matrix4f ext_T = Eigen::Matrix4f::Identity();
    ext_T.block<3, 3>(0, 0) = ext_R;
    ext_T.block<3, 1>(0, 3) = Eigen::Vector3f::Ones() * baseline;
    T01 = T01 * ext_T;

    Eigen::Vector3f p0(612, 171, 1); // 齐次相机坐标系下的点(u, v, 1), 左相机点
    Eigen::Vector3f p1(614.431, 172.358, 1);
    Eigen::Vector3f t1 = T01.block<3, 1>(0, 3);
    Eigen::Matrix3f E = hat(t1) * T01.block<3, 3>(0, 0);
    Eigen::Vector3f x0 = K.inverse() * p0;
    Eigen::Vector3f x1 = K.inverse() * p1;

    LOG_TEST("xo^T * E * x1: ", x0.transpose() * E * x1);
}

Eigen::Matrix3f hat(const Eigen::Vector3f& v)
{
    Eigen::Matrix3f hat_v;
    hat_v << 0, -v(2), v(1),
             v(2), 0, -v(0),
             -v(1), v(0), 0;
    return hat_v;
}