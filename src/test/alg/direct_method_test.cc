#include "core/tt_test.h"
#include "common/log.h"

#include "factor_graph/linear_regression_optimize.h"

#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

class DirectMethodFactor : public LinearRegressionFactor
{
public:
    DirectMethodFactor(LinearRegressionVariable *v_a, const Eigen::VectorXd& measurement,
                          const Eigen::Matrix3d& K, const cv::Mat& img0, const cv::Mat& img1) 
        : LinearRegressionFactor(v_a, measurement), K_(K), K_inv_(K.inverse()), img0_(img0), img1_(img1)
    {}
    
    virtual Eigen::VectorXd Error() const override
    {
        GRAPH_ASSERT(this->NumVariables() == 1);
        const LinearRegressionVariable *v_a = static_cast<LinearRegressionVariable *>(this->VariableAt(0));

        // 像素坐标转相机坐标
        Eigen::Vector2d p0_pixel = measurement_;
        Eigen::Vector3d p0 = K_inv_ * Eigen::Vector3d(p0_pixel(0), p0_pixel(1), 1);

        // 相机坐标转世界坐标        
        Eigen::Quaterniond q(1, v_a->x()(0), v_a->x()(1), v_a->x()(2));
        Eigen::Matrix3d R = q.normalized().toRotationMatrix();
        Eigen::Vector3d t = v_a->x().tail(3);

        Eigen::Vector3d p1 = R * p0 + t;

        // 相机坐标转像素坐标
        Eigen::Vector3d p1_pixel = K_ * p1;
        Eigen::Vector2d p1_pixel_norm = p1_pixel.head(2) / p1_pixel(2);

        // 双线性插值 + patch
        double error = 0;
        constexpr uint patch_size = 4;
        for(uint i = 0; i < patch_size; ++i)
        {
            for(uint j = 0; j < patch_size; ++j)
            {
                double x0 = p0_pixel(0) + i - patch_size / 2;
                double y0 = p0_pixel(1) + j - patch_size / 2;

                double x1 = p1_pixel_norm(0) + i - patch_size / 2;
                double y1 = p1_pixel_norm(1) + j - patch_size / 2;

                double v0 = get_value(x0, y0, img0_);
                double v1 = get_value(x1, y1, img1_);

                error += (v0 - v1) * (v0 - v1);
            }
        }

        return Eigen::VectorXd::Ones(1) * error;
    }
private:
    double get_value(double x, double y, const cv::Mat& img) const
    {
        int width = img.cols;
        int height = img.rows;

        if(x < 0) x = 0;
        if(x > width - 1) x = width - 1;
        if(y < 0) y = 0;
        if(y > height - 1) y = height - 1;

        int x0 = (int)x;
        int y0 = (int)y;
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        double v00 = img.at<uchar>(y0, x0);
        double v01 = img.at<uchar>(y0, x1);
        double v10 = img.at<uchar>(y1, x0);
        double v11 = img.at<uchar>(y1, x1);

        double v0 = (x1 - x) * v00 + (x - x0) * v01;
        double v1 = (x1 - x) * v10 + (x - x0) * v11;

        return (y1 - y) * v0 + (y - y0) * v1;
    }
private:
    const Eigen::Matrix3d& K_;
    const Eigen::Matrix3d& K_inv_;
    const cv::Mat& img0_;
    const cv::Mat& img1_;
};


// JUST_RUN_TEST(direct_method, test)
TEST(direct_method, test)
{
    LOG_TEST("------------------------------------direct method test-------------------------------------");

    cv::Mat img0 = cv::imread("../src/data/l0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img1 = cv::imread("../src/data/l1.png", cv::IMREAD_GRAYSCALE);

    Eigen::Matrix3d K;
    K << 718.856, 0, 607.1928,
         0, 718.856, 185.2157,
         0, 0, 1;
    
    // Eigen::Matrix4d T = Eigen::Matrix4d::Identity();

}