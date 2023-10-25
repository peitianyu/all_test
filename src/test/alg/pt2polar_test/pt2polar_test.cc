#include"core/tt_test.h"
#include "common/log.h"
#include "core/tt_tictoc.h"
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <vector>
#include <utility>
#include "pt2polar_factor.h"
#include "factor_graph/graph_optimize.h"



using PairFeat = std::pair<cv::Point2f, cv::Point2f>;
std::vector<PairFeat> stereo_detect(const cv::Mat& left_img, const cv::Mat& right_img);
void tracking(const std::vector<PairFeat>& fts, const Eigen::Matrix<float, 3, 3>& cam_info, cv::Mat& R, cv::Mat& t);
void optimize(const cv::Mat& R, const cv::Mat& t);

std::vector<PairFeat> fts_r1l0;
std::vector<PairFeat> fts_l1r0;
std::vector<PairFeat> fts_r1r0;

JUST_RUN_TEST(pt2polar, test)
TEST(pt2polar, test)
{
    float fx = 718.856, fy = 718.856, cx = 607.1928, cy = 185.2157;
    Eigen::Matrix<float, 3, 3> cam_info;
    cam_info << fx, 0, cx,
                 0, fy, cy,
                 0, 0, 1;

    cv::Mat img_l0 = cv::imread("../src/data/l0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_l1 = cv::imread("../src/data/l1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_r0 = cv::imread("../src/data/r0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_r1 = cv::imread("../src/data/r1.png", cv::IMREAD_GRAYSCALE);

    // 对极约束估计R, t
    cv::Mat R, t;
    std::vector<PairFeat> fts_l0l1 = stereo_detect(img_l0, img_l1);
    tracking(fts_l0l1, cam_info, R, t);

    fts_r1l0 = stereo_detect(img_r1, img_l0);
    fts_l1r0 = stereo_detect(img_l1, img_r0);
    fts_r1r0 = stereo_detect(img_r1, img_r0);

    optimize(R, t);
}

void optimize(const cv::Mat& rvec, const cv::Mat& tvec)
{
    TicTocAuto tictoc_auto("optimize");
    Eigen::Matrix3d dR;
    Eigen::Vector3d dt;
    cv::cv2eigen(rvec, dR);
    cv::cv2eigen(tvec, dt);

    FactorGraph graph;
    Pt2PolarVariable *v_a = new Pt2PolarVariable(Eigen::Vector4d(0, 0, 0, 0.68)); // alpha, beta, gamma, scale
    graph.AddVariable(v_a);

    Eigen::Matrix4d T = Eigen::Matrix4d::Identity();
    T.block<3, 3>(0, 0) = dR.transpose();
    T.block<3, 1>(0, 3) = -dR.transpose() * dt;

    Eigen::Matrix3d K;
    K << 718.856, 0, 607.1928,
         0, 718.856, 185.2157,
         0, 0, 1;
    double base_line = 0.386145;
    
    for(uint i = 0; i < fts_r1l0.size(); i++) {
        Eigen::Vector3d p0 = Eigen::Vector3d(fts_r1l0[i].first.x, fts_r1l0[i].first.y, 1);
        Eigen::Vector3d p1 = Eigen::Vector3d(fts_r1l0[i].second.x, fts_r1l0[i].second.y, 1); 
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, base_line, K, p0, p1, 0); // p1 for line, p0 for point, // 3种模式, 代表三种坐标变换方式
        graph.AddFactor(f);
    }

    for(uint i = 0; i < fts_l1r0.size(); i++) {
        Eigen::Vector3d p0 = Eigen::Vector3d(fts_l1r0[i].first.x, fts_l1r0[i].first.y, 1);
        Eigen::Vector3d p1 = Eigen::Vector3d(fts_l1r0[i].second.x, fts_l1r0[i].second.y, 1);
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, base_line, K, p0, p1, 1); // p1 for line, p0 for point
        graph.AddFactor(f);
    }

    for(uint i = 0; i < fts_r1r0.size(); i++) {
        Eigen::Vector3d p0 = Eigen::Vector3d(fts_r1r0[i].first.x, fts_r1r0[i].first.y, 1);
        Eigen::Vector3d p1 = Eigen::Vector3d(fts_r1r0[i].second.x, fts_r1r0[i].second.y, 1);
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, base_line, K, p0, p1, 2); // p1 for line, p0 for point
        graph.AddFactor(f);
    }

    // 优化
    GraphOptimize::Option option = GraphOptimize::Option();
    GraphOptimize graph_optimize = GraphOptimize(option);

    graph_optimize.OptimizeGN(&graph);

    v_a->Print();

    // 对T修正
    Eigen::Matrix4d T01 = Eigen::Matrix4d::Identity();
    T01 << T(0,0), T(0,1), T(0,2), T(0,3) * v_a->scale(),
           T(1,0), T(1,1), T(1,2), T(1,3) * v_a->scale(),
           T(2,0), T(2,1), T(2,2), T(2,3) * v_a->scale(),
           0, 0, 0, 1;

    std::cout << "T01: " << std::endl << T01 << std::endl;
}


void visualize(const uint& id, const cv::Mat& img, const std::vector<PairFeat>& fts)
{
    cv::Mat v_img;
    cv::cvtColor(img, v_img, cv::COLOR_GRAY2BGR);
    for(uint i = 0; i < fts.size(); i++) {
        cv::circle(v_img, fts[i].first, 2, cv::Scalar(0, 0, 255), -1);
        cv::line(v_img, fts[i].first, fts[i].second, cv::Scalar(0, 255, 0));
    }
    cv::imshow("v_img"+std::to_string(id), v_img);
}

void remove_outliers(std::vector<cv::Point2f> &feats_prev, std::vector<cv::Point2f> &feats_curr);
std::vector<PairFeat> stereo_detect(const cv::Mat& left_img, const cv::Mat& right_img)
{
    int thresh = 10;
    // cv::Mat mask = cv::Mat(left_img.size(), CV_8UC1, cv::Scalar(255));
    cv::Ptr<cv::FastFeatureDetector> detector = cv::FastFeatureDetector::create(thresh);
    std::vector<cv::KeyPoint> left_keypoints;
    detector->detect(left_img, left_keypoints);
    
    static auto cmp = [](const cv::KeyPoint& a, const cv::KeyPoint& b) -> bool { return a.response > b.response; };
    std::sort(left_keypoints.begin(), left_keypoints.end(), cmp);
    std::vector<cv::Point2f> left_feats, right_feats;
    for(uint i = 0; i < left_keypoints.size(); i++) {
        if(left_feats.size() >= 500) break; 
        
        left_feats.push_back(left_keypoints[i].pt);
    }
    
    std::vector<uchar> status;
    std::vector<float> err;
    cv::calcOpticalFlowPyrLK(left_img, right_img, left_feats, right_feats, status, err);

    remove_outliers(left_feats, right_feats);

    std::vector<PairFeat> feats_tracked;
    for(uint i = 0; i < status.size(); i++) {
        if(!status[i]) continue;
        if(err[i] > 10) continue;

        feats_tracked.emplace_back(left_feats[i], right_feats[i]);
    }
    return feats_tracked;
}

void remove_outliers(std::vector<cv::Point2f> &feats_prev, std::vector<cv::Point2f> &feats_curr)
{
    std::vector<uchar> status;
    cv::findFundamentalMat(feats_prev, feats_curr, cv::FM_RANSAC, 1.0, 0.99, status);

    uint j = 0;
    for(uint i = 0; i < status.size(); i++) {
        if(!status[i]) continue;

        feats_prev[j] = feats_prev[i];
        feats_curr[j] = feats_curr[i];

        j++;
    }

    feats_prev.resize(j);
    feats_curr.resize(j);
}

void update_odom(const cv::Mat& rvec, const cv::Mat& tvec);

void tracking(const std::vector<PairFeat>& fts, const Eigen::Matrix<float, 3, 3>& cam_info, cv::Mat& R, cv::Mat& t)
{
    // 1. 求解本质矩阵
    std::vector<cv::Point2f> pts1, pts2;
    for(uint i = 0; i < fts.size(); i++) {
        pts1.push_back(fts[i].first);
        pts2.push_back(fts[i].second);
    }
    
    cv::Mat cam_mat = cv::Mat::zeros(3, 3, CV_32FC1);
    cam_mat.at<float>(0, 0) = cam_info(0, 0);
    cam_mat.at<float>(1, 1) = cam_info(1, 1);
    cam_mat.at<float>(0, 2) = cam_info(0, 2);
    cam_mat.at<float>(1, 2) = cam_info(1, 2);
    cv::Mat E = cv::findEssentialMat(pts1, pts2, cam_mat, cv::RANSAC, 0.999, 1.0);

    // 2. 求解R, t
    cv::recoverPose(E, pts1, pts2, cam_mat, R, t);

    update_odom(R, t);
}

void update_odom(const cv::Mat& rvec, const cv::Mat& tvec)
{
    Eigen::Matrix3d dR;
    Eigen::Vector3d dt;
    cv::cv2eigen(rvec, dR);
    cv::cv2eigen(tvec, dt);

    static Eigen::Quaterniond q = Eigen::Quaterniond::Identity();
    static Eigen::Vector3d t = Eigen::Vector3d::Zero();
    dt = -dR.transpose() * dt;
    Eigen::Quaterniond dq(dR.transpose());
    t = t + q.toRotationMatrix() * dt;
    q = q * dq;

    // Eigen::Vector3d euler_angle = q.toRotationMatrix().eulerAngles(2, 1, 0);
    LOG_TEST("vo1: ", t.transpose());

    // scale大概在0.68左右
}
