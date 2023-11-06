#include <opencv2/opencv.hpp>
#include <Eigen/Core>

class ImagePyramid
{
public:
    ImagePyramid(const cv::Mat& K, const cv::Mat& img) : K_(K), img_(img)
    {}

    ImagePyramid(const ImagePyramid& father) : K_(father.K_*0.5)
    {
        K_.at<double>(2, 2) = 1.0;
        cv::pyrDown(father.img_, img_);
        // cv::resize(father.img_, img_, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
    }

    const cv::Mat& img() const { return img_; } 
    const cv::Mat& K() const { return K_; }
    Eigen::Matrix3d K_eigen() const {
        Eigen::Matrix3d K = Eigen::Matrix3d::Identity();
        K(0, 0) = K_.at<double>(0, 0);
        K(1, 1) = K_.at<double>(1, 1);
        K(0, 2) = K_.at<double>(0, 2);
        K(1, 2) = K_.at<double>(1, 2);
        return K;
    }

    double fx() const { return K_.at<double>(0, 0); }
    double fy() const { return K_.at<double>(1, 1); }
    double cx() const { return K_.at<double>(0, 2); }
    double cy() const { return K_.at<double>(1, 2); }
private:
    cv::Mat K_;
    cv::Mat img_;
};