#include "core/tt_test.h"
#include "common/log.h"


#include <sophus/so3.hpp>

#include "common/pose3d.h"

static Eigen::Quaterniond unit_random() {
    double u1 = rand() / double(RAND_MAX); // [0, 1]
    double u2 = rand() / double(RAND_MAX) * M_2_PI;
    double u3 = rand() / double(RAND_MAX) * M_2_PI;
    double a = std::sqrt(1 - u1);
    double b = std::sqrt(u1);
    return Eigen::Quaterniond(a*sin(u2), a*cos(u2), b*sin(u3), b*cos(u3)).normalized();
}

// JUST_RUN_TEST(pose, base)
TEST(pose, base)
{
    LOG_TEST("/*************** ch01_rot.cc ***************/");

    Eigen::Matrix3d R(unit_random());
    // LOG_TEST("the random initial rotation matrix R:\n", R);

    Eigen::Vector3d omega(0.01, 0.02, 0.03);
    Eigen::Quaterniond q_update(1, omega[0]*0.5, omega[1]*0.5, omega[2]*0.5);
    // 打印四元数
    LOG_TEST("q_update: ", q_update.coeffs().transpose());

    LOG_TEST("/**************** Quaterniond ****************/");
    Eigen::Quaterniond R_q(R);
    Eigen::Quaterniond R_q_updated = (R_q*q_update).normalized();
    Eigen::Matrix3d Quaterniond_R = R_q_updated.toRotationMatrix();
    LOG_TEST("Quaterniond_R:\n", Quaterniond_R);

    LOG_TEST("/**************** SO3 ****************/");
    Sophus::SO3<double> R_SO3(R);
    Sophus::SO3<double> R_SO3_updated = R_SO3 * Sophus::SO3<double>::exp(omega);
    Eigen::Matrix3d SO3_R = R_SO3_updated.matrix();
    LOG_TEST("SO3_R:\n", SO3_R);
}

JUST_RUN_TEST(ch01, pose3d)
TEST(ch01, pose3d)
{
    LOG_TEST("/*************** ch01_pose ***************/");

    Eigen::Matrix3d R(unit_random());
    Eigen::Vector3d omega(0.01, 0.02, 0.03);
    Eigen::Quaterniond q_update(1, omega[0]*0.5, omega[1]*0.5, omega[2]*0.5);

    LOG_TEST("/**************** Pose3d ****************/");
    Pose3d p(R, Eigen::Vector3d(1, 2, 3));
    Pose3d p_update(q_update, Eigen::Vector3d(1, 2, 3));

    Pose3d p_updated = p.trans_add(p_update);
    LOG_TEST("pose3d: \n", p_updated);

    Pose3d p1 = p.trans_from(p_updated);
    LOG_TEST("pose3d1: \n", p.trans_add(p1));

    LOG_TEST("/**************** slerp ****************/");
    Pose3d p_slerp = p_update * 0.5;
    LOG_TEST("pose3d: \n", p.trans_add(p_slerp));


    LOG_TEST("/**************** Point3d ****************/");
    Point3d point(1, 2, 3);
    LOG_TEST("updated_point: ", p.trans_add(point).transpose());
    LOG_TEST("point_from: ", p.trans_from(point).transpose());
}

