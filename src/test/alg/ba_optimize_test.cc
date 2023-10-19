#include "core/tt_test.h"
#include "common/log.h"

#include "common/ba_optimize.h"


static void read_data(const std::string& p3d_path, const std::string& p2d_path,
                      std::vector<Eigen::Vector3d>& p3ds, std::vector<Eigen::Vector2d>& p2ds);

JUST_RUN_TEST(ba_optimize, test)
TEST(ba_optimize, test)
{
    LOG_TEST("test");

    Eigen::Matrix3d camera_info = Eigen::Matrix3d::Identity();

    // double fx = 520.9, fy = 521.0, cx = 325.1, cy = 249.7;
    camera_info << 520.9, 0, 325.1,
                   0, 521.0, 249.7,
                   0, 0, 1;

    BA_Optimize ba_optimize(camera_info);

    std::vector<Eigen::Vector2d> p2ds;
    std::vector<Eigen::Vector3d> p3ds;
    std::vector<Eigen::Matrix4d> poses;
    poses.push_back(Eigen::Matrix4d::Identity());


    read_data("../src/data/p3d.txt", "../src/data/p2d.txt", p3ds, p2ds);

    ba_optimize.optimize(p2ds, p3ds, poses);

    LOG_TEST("pose: \n", poses[0]);
}


#include <fstream>
static void read_data(const std::string& p3d_path, const std::string& p2d_path,
                      std::vector<Eigen::Vector3d>& p3ds, std::vector<Eigen::Vector2d>& p2ds)
{
    std::ifstream f3d(p3d_path);
    if(!f3d)
    {
        LOG("No file for 3D points.");
    }
    else {
        while(!f3d.eof())
        {
            double pt3[3] = {0};
            f3d >> pt3[0] >> pt3[1] >> pt3[2];
            Eigen::Vector3d Point3d;
            Point3d << pt3[0], pt3[1], pt3[2];
            p3ds.push_back(Point3d);
        }
    }

    std::ifstream f2d(p2d_path);
    if (!f2d) {
        LOG("No file for 2D pixels.");
    } else {
        while (!f2d.eof()) {
            double pt2[2] = {0};
            f2d >> pt2[0] >> pt2[1];
            Eigen::Vector2d Point2d;
            Point2d << pt2[0], pt2[1];
            p2ds.push_back(Point2d);
        }
    }
    assert(p3ds.size() == p2ds.size());
    LOG("The test data contains %d 3D-2D pairs.", p3ds.size());
}