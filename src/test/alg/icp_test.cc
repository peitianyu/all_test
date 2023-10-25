#include "core/tt_test.h"
#include "common/log.h"
#include "common/icp.h"
#include "core/tt_tictoc.h"
#include <Eigen/SVD>
#include <iomanip>

static void random_point_cloud(std::vector<Eigen::Vector3d>& points, int num);
std::vector<Eigen::Vector3d> update_point_cloud(const std::vector<Eigen::Vector3d>& points, const Eigen::Matrix4d& pose);
void pose_estimation_3d3d(const std::vector<Eigen::Vector3d>& pts1, const std::vector<Eigen::Vector3d>& pts2, Eigen::Matrix4d& pose);

// JUST_RUN_TEST(icp, base)
TEST(icp, base)
{
    std::vector<Eigen::Vector3d> ref_pts, cur_pts;
    random_point_cloud(ref_pts, 500);
    Eigen::Matrix4d true_pose = (Eigen::Matrix4d() <<   1, 0, 0, 0.1,
                                                        0, 1, 0, 0.2,
                                                        0, 0, 1, 0.3,
                                                        0, 0, 0, 1).finished();
    cur_pts = update_point_cloud(ref_pts, true_pose);

    // for(const auto& p : ref_pts) LOG_TEST("ref: ", p.transpose());
        
    ICP icp;
    Eigen::Matrix4d pose = Eigen::Matrix4d::Identity();
    icp.optimize(ref_pts, cur_pts, pose);
    LOG_TEST("optimize pose: \n", pose);

    pose = Eigen::Matrix4d::Identity();
    pose_estimation_3d3d(ref_pts, cur_pts, pose);

    LOG_TEST("svd pose: \n", pose);
}

static void random_point_cloud(std::vector<Eigen::Vector3d>& points, int num)
{
    points.clear();
    for (int i = 0; i < num; ++i)
    {
        Eigen::Vector3d p;
        p.setRandom();
        points.push_back(p*10);
    }
}

std::vector<Eigen::Vector3d> update_point_cloud(const std::vector<Eigen::Vector3d>& points, const Eigen::Matrix4d& pose)
{
    std::vector<Eigen::Vector3d> new_points;
    for (uint i = 0; i < points.size(); ++i)
    {
        Eigen::Vector3d p = pose.block(0, 0, 3, 3) * points[i] + pose.block(0, 3, 3, 1);
        new_points.push_back(p);
    }
    return new_points;
}

void pose_estimation_3d3d(const std::vector<Eigen::Vector3d>& pts1, const std::vector<Eigen::Vector3d>& pts2, Eigen::Matrix4d& pose) 
{
    TicTocAuto tictoc("pose_estimation_3d3d");
    // 求质心坐标
    Eigen::Vector3d p1, p2;
    int N = pts1.size();
    for(int i=0; i<N; i++) {
        p1 += pts1[i];
        p2 += pts2[i];
    }
    p1 = p1 / N;
    p2 = p2 / N;

    // 求去质心坐标
    std::vector<Eigen::Vector3d> q1(N), q2(N);
    for(int i=0; i<N; i++) {
        q1[i] = pts1[i] - p1;
        q2[i] = pts2[i] - p2;
    }

    // 求和: W = E(q1 * q2^T)
    Eigen::Matrix3d W = Eigen::Matrix3d::Zero();
    for(int i=0; i<N; i++) {
        W += q1[i] * q2[i].transpose();
    }
    std::cout << "W=" << std::endl << W << std::endl << std::endl;

    // 对W进行SVD分解
    Eigen::JacobiSVD<Eigen::Matrix3d> svd(W, Eigen::ComputeFullU|Eigen::ComputeFullV);
    Eigen::Matrix3d U = svd.matrixU();
    Eigen::Matrix3d V = svd.matrixV();
    
    // 求R
    Eigen::Matrix3d dR = U * (V.transpose());
    Eigen::Vector3d dt = p1 - dR * p2;

    pose.block(0, 0, 3, 3) = dR * pose.block(0, 0, 3, 3);
    pose.block(0, 3, 3, 1) += dt;

    std::cout << "pose=" << std::endl << pose << std::endl;
}