#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <Eigen/SVD>

#include "core/tt_test.h"
#include "common/log.h"

using namespace std;
using namespace cv;

void pose_estimation_3d3d(const vector<Point3f>& pts1,
			  const vector<Point3f>& pts2) {
	// 求质心坐标
	Point3f p1, p2;
	int N = pts1.size();
	for(int i=0; i<N; i++) {
		p1 += pts1[i];
		p2 += pts2[i];
	}
	p1 = Point3f(Vec3f(p1) / N);
	p2 = Point3f(Vec3f(p2) / N);

	// 求去质心坐标
	vector<Point3f> q1(N), q2(N);
	for(int i=0; i<N; i++) {
		q1[i] = pts1[i] - p1;
		q2[i] = pts2[i] - p2;
	}

	// 求和: W = E(q1 * q2^T)
	Eigen::Matrix3d W = Eigen::Matrix3d::Zero();
	for(int i=0; i<N; i++) {
		W += Eigen::Vector3d(q1[i].x, q1[i].y, q1[i].z) *
		     Eigen::Vector3d(q2[i].x, q2[i].y, q2[i].z).transpose();
	}
	cout << "W=" << endl << W << endl << endl;

	// 对W进行SVD分解
	Eigen::JacobiSVD<Eigen::Matrix3d> svd(W, Eigen::ComputeFullU|Eigen::ComputeFullV);
	Eigen::Matrix3d U = svd.matrixU();
	Eigen::Matrix3d V = svd.matrixV();
	cout << "U=" << endl << U << endl << endl;
	cout << "V=" << endl << V << endl << endl;

	// 求R
	Eigen::Matrix3d R = U * (V.transpose());
	cout << "R=" << endl << fixed << setprecision(2) << R << endl << endl;

	// 求t
	Eigen::Vector3d t = Eigen::Vector3d(p1.x, p1.y, p1.z) - R * Eigen::Vector3d(p2.x, p2.y, p2.z);
	cout << "t=" << endl << t << endl;
}

// JUST_RUN_TEST(icp_svd, svd)
TEST(icp_svd, svd)
{
    vector<Point3f> pts1, pts2;
	pts1.push_back(Point3f(2, 3, 1));
	pts1.push_back(Point3f(2, -2, 3));
	pts1.push_back(Point3f(3, -2, 2));
	pts1.push_back(Point3f(1, -3, 1));
	pts1.push_back(Point3f(4, 0, 2));
	pts1.push_back(Point3f(0, -8, 3));
	
	pts2.push_back(Point3f(-4, 2, 1));
	pts2.push_back(Point3f(1, 2, 3));
	pts2.push_back(Point3f(1, 3, 2));
	pts2.push_back(Point3f(2, 1, 1));
	pts2.push_back(Point3f(-1, 4, 2 ));
	pts2.push_back(Point3f(7, 0, 3));

	pose_estimation_3d3d(pts1, pts2);
}