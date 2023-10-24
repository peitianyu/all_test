#include"core/tt_test.h"
#include "common/log.h"
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
void tracking(const std::vector<PairFeat>& fts, const Eigen::Matrix<float, 3, 4>& cam_info, cv::Mat& R, cv::Mat& t);
void optimize(const cv::Mat& R, const cv::Mat& t);

std::vector<PairFeat> fts_l0r1;
std::vector<PairFeat> fts_r0l1;
std::vector<PairFeat> fts_r0r1;

JUST_RUN_TEST(pt2polar, test)
TEST(pt2polar, test)
{
    float fx = 0.718856, fy = 0.718856, cx = 0.6071928, cy = 0.1852157, bl = 0.386145;
    Eigen::Matrix<float, 3, 4> cam_info;
    cam_info << fx, 0, cx, bl*fx,
                 0, fy, cy, 0,
                 0, 0, 1, 0;

    cv::Mat img_l0 = cv::imread("../src/data/l0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_r0 = cv::imread("../src/data/r0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_l1 = cv::imread("../src/data/l1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img_r1 = cv::imread("../src/data/r1.png", cv::IMREAD_GRAYSCALE);

    // 对极约束估计R, t
    cv::Mat R, t;
    std::vector<PairFeat> fts_l0l1 = stereo_detect(img_l0, img_l1);
    tracking(fts_l0l1, cam_info, R, t);

    fts_l0r1 = stereo_detect(img_l0, img_r1);
    fts_r0l1 = stereo_detect(img_r0, img_l1);
    fts_r0r1 = stereo_detect(img_r0, img_r1);

    optimize(R, t);
}

void optimize(const cv::Mat& R, const cv::Mat& t)
{
    FactorGraph graph;
    Pt2PolarVariable *v_a = new Pt2PolarVariable(Eigen::Vector4d(0, 0, 0, 1));
    graph.AddVariable(v_a);

    Eigen::Matrix4d T = Eigen::Matrix4d::Identity();
    T << R.at<double>(0,0), R.at<double>(0,1), R.at<double>(0,2), t.at<double>(0),
         R.at<double>(1,0), R.at<double>(1,1), R.at<double>(1,2), t.at<double>(1),
         R.at<double>(2,0), R.at<double>(2,1), R.at<double>(2,2), t.at<double>(2),
         0, 0, 0, 1;
        
    for(uint i = 0; i < fts_l0r1.size(); i++) {
        Eigen::Vector3d p0(fts_l0r1[i].first.x, fts_l0r1[i].first.y, 1);
        Eigen::Vector3d p1(fts_l0r1[i].second.x, fts_l0r1[i].second.y, 1);
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, 0.386145, p0, p1);
        graph.AddFactor(f);
    }

    for(uint i = 0; i < fts_r0l1.size(); i++) {
        Eigen::Vector3d p0(fts_r0l1[i].first.x, fts_r0l1[i].first.y, 1);
        Eigen::Vector3d p1(fts_r0l1[i].second.x, fts_r0l1[i].second.y, 1);
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, 0.386145, p0, p1);
        graph.AddFactor(f);
    }

    for(uint i = 0; i < fts_r0r1.size(); i++) {
        Eigen::Vector3d p0(fts_r0r1[i].first.x, fts_r0r1[i].first.y, 1);
        Eigen::Vector3d p1(fts_r0r1[i].second.x, fts_r0r1[i].second.y, 1);
        Pt2PolarFactor *f = new Pt2PolarFactor(v_a, T, 0.386145, p0, p1);
        graph.AddFactor(f);
    }

    // 优化
    GraphOptimize::Option option = GraphOptimize::Option();
    GraphOptimize graph_optimize = GraphOptimize(option);

    graph_optimize.OptimizeGN(&graph);

    v_a->Print();

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

    std::vector<PairFeat> feats_tracked;
    for(uint i = 0; i < status.size(); i++) {
        if(!status[i]) continue;
        if(err[i] > 10) continue;

        feats_tracked.emplace_back(left_feats[i], right_feats[i]);
    }
    return feats_tracked;
}

void tracking(const std::vector<PairFeat>& fts, const Eigen::Matrix<float, 3, 4>& cam_info, cv::Mat& R, cv::Mat& t)
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
}
