#include "core/tt_test.h"
#include "common/log.h"
#include "common/icp.h"


static void random_point_cloud(std::vector<Eigen::Vector3d>& points, int num);
std::vector<Eigen::Vector3d> update_point_cloud(const std::vector<Eigen::Vector3d>& points, const Eigen::Matrix4d& pose);

JUST_RUN_TEST(icp, base)
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

    LOG_TEST("pose: \n", pose);
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