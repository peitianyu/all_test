#ifndef __POINT_CLOUD_VISUAL_H
#define __POINT_CLOUD_VISUAL_H

#include <opencv2/opencv.hpp>
#include <opencv2/viz.hpp>
#include <Eigen/Core>
#include <vector>
#include <iostream>
#include <fstream>

class PointCloudVisual
{
public:
    PointCloudVisual() = default;

    void operator()(const std::string& point_cloud_path)
    {
        m_window.setBackgroundColor(cv::viz::Color::white());

        m_window.showWidget("PointCloud", cv::viz::WCloud(Points2Mat(ReadPointData(point_cloud_path)), cv::viz::Viz3d::Color::red()));

        m_window.spin();
    }
private:
    std::vector<Eigen::Vector3f> ReadPointData(const std::string &point_cloud_path)
    {
        std::vector<Eigen::Vector3f> point_cloud;
        std::ifstream fin(point_cloud_path);
        if (!fin.is_open())
        {
            std::cerr << "Cannot open file: " << point_cloud_path << std::endl;
            return point_cloud;
        }
        std::string line;
        while (std::getline(fin, line))
        {
            std::stringstream ss(line);
            float x, y, z;
            ss >> x >> y >> z;
            point_cloud.emplace_back(x, y, z);
        }
        return point_cloud;
    }

    cv::Mat Points2Mat(const std::vector<Eigen::Vector3f> &points)
    {
        cv::Mat point_cloud_mat = cv::Mat::zeros(3, points.size(), CV_32FC3);
        for (size_t i = 0; i < points.size(); ++i)
        {
            point_cloud_mat.ptr<cv::Vec3f>(0)[i][0] = points[i][0];
            point_cloud_mat.ptr<cv::Vec3f>(0)[i][1] = points[i][1];
            point_cloud_mat.ptr<cv::Vec3f>(0)[i][2] = points[i][2];
        }
        return point_cloud_mat;
    }
private:
    cv::viz::Viz3d m_window;
};




#endif