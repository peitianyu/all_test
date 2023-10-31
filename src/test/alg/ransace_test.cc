#include "core/tt_test.h"
#include "common/log.h"
#include "common/read_file.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <random>

void solve_plane(const Eigen::Vector3d& p0, const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, Eigen::Vector4d& n) // n: ax+by+cz+d=0
{
    Eigen::Vector3d N = (p1 - p0).cross(p2 - p0);
    N.normalize();
    n = Eigen::Vector4d(N(0), N(1), N(2), -N.dot(p0));
}

double solve_distance(const Eigen::Vector3d& p, const Eigen::Vector4d& n)
{
    return std::abs(n.head(3).dot(p) + n(3)) / n.head(3).norm();
}

std::vector<Eigen::Vector3d> read_data(const std::string& path);

JUST_RUN_TEST(ransac, test)
TEST(ransac, test)
{
    std::vector<Eigen::Vector3d> pts = read_data("../src/data/pcltest.csv");

    // 保留前1000个点
    pts.erase(pts.begin() + 2000, pts.end());

    LOG_TEST("--------------------ransac--------------------");
    LOG_TEST("--------------------test data: ", pts.size(), "--------------------");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, pts.size());

    unsigned long size = pts.size();
    uint iters = 10000;
    double sigma = 0.1; 
    uint pretotal = 0;
    double per = 0.999;
    Eigen::Vector4d n = Eigen::Vector4d::Zero();
    for (uint i = 0; i < iters; i++){
        int index[3] = {dis(gen), dis(gen), dis(gen)};

        if (index[0] == index[1] || index[1] == index[2] || index[2] == index[0]){
            i--;
            continue;
        }

        solve_plane(pts[index[0]], pts[index[1]], pts[index[2]], n);

        uint total_inlier = 0;
        for (uint j = 0; j < size; j++)
            total_inlier += (solve_distance(pts[j], n) < sigma);

        if (total_inlier > pretotal){
            iters = int(log(1.0 - per) / log(1.0 - pow(total_inlier / size, 2)));
            pretotal = total_inlier;
        }

        if (total_inlier > size*0.5) break;
    }

    LOG_TEST("n: ", n.transpose());
    LOG_TEST("pretotal: ", pretotal);
}


// JUST_RUN_TEST(ransac_plane, test)
TEST(ransac_plane, test)
{
    std::vector<Eigen::Vector3d> pts = read_data("../src/data/pcltest.csv");

    Eigen::Vector4d n;
    solve_plane(pts[0], pts[1], pts[2], n);
    for(uint i = 0; i < 3; i++){
        LOG_TEST("pts[", i, "]: ", pts[i].transpose());
    }

    LOG_TEST("n: ", n.transpose());

    Eigen::Vector3d p(0, 0, 0);
    double d = solve_distance(p, n);

    LOG_TEST("d: ", d);
}

std::vector<Eigen::Vector3d> read_data(const std::string& path)
{
    ReadFile rf(path);
    std::vector<Eigen::Vector3d> pts;

    while(!rf.eof()) {
        std::stringstream ss = rf.get_stringstream();
        std::string str;
        Eigen::Vector3d pt;
        for (int i = 0; i < 3; i++) {
            getline(ss, str, ',');
            pt(i) = stod(str);
        }
        pts.push_back(pt);
    }

    return pts;
}
