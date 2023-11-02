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
        : LinearRegressionFactor(v_a, measurement), K_(K), img0_(img0), img1_(img1)
    {
        K_inv_ = K_.inverse();
    }
    
    virtual Eigen::VectorXd Error() const override
    {
        GRAPH_ASSERT(this->NumVariables() == 1);
        const LinearRegressionVariable *v_a = static_cast<LinearRegressionVariable *>(this->VariableAt(0));

        Eigen::Vector3d x =  v_a->x().head<3>();
        Eigen::Matrix3d R = Eigen::AngleAxisd(x.norm(), x.normalized()).toRotationMatrix();
        Eigen::Vector3d t = v_a->x().tail(3);

        // 像素坐标 -> 相机坐标 -> 世界坐标
        Eigen::Vector2d p0_pixel = measurement_;
        Eigen::Vector3d p0 = K_ * Eigen::Vector3d(p0_pixel(0), p0_pixel(1), 1);
        Eigen::Vector3d p1 = R * p0 + t;
        Eigen::Vector3d p1_pixel = K_inv_ * p1;
        Eigen::Vector2d p1_pixel_norm = p1_pixel.head(2) / p1_pixel(2);

        // std::cout << "p0_pixel: " << p0_pixel.transpose() << "  p1_pixel: " << p1_pixel_norm.transpose() << std::endl;

        // 双线性插值 + patch
        double error = 0;
        constexpr uint patch_size = 3;
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

        // std::cout << "error: " << error << std::endl;

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
    Eigen::Matrix3d K_;
    Eigen::Matrix3d K_inv_;
    const cv::Mat& img0_;
    const cv::Mat& img1_;
};

class DirectMethodOptimize : public LinearRegressionOptimize<DirectMethodFactor>
{
public:
    void Optimize(const Eigen::VectorXd& X, const Eigen::MatrixXd& measurement, const Eigen::Matrix3d& K, const cv::Mat& img0, const cv::Mat& img1)
    {
        FactorGraph factor_graph;
        LinearRegressionVariable *v_x = new LinearRegressionVariable(X);
        factor_graph.AddVariable(v_x);
        for(int i = 0; i < measurement.rows(); i++)
        {
            DirectMethodFactor *factor = new DirectMethodFactor(v_x, measurement.row(i), K, img0, img1);
            factor_graph.AddFactor(factor);
        }

        GraphOptimize graph_optimize = GraphOptimize(GraphOptimize::Option());
        graph_optimize.OptimizeGN(&factor_graph);

        X_ = v_x->x();
    }
};

std::vector<Eigen::Vector2d> get_good_features(const cv::Mat& img);


JUST_RUN_TEST(direct_method, test)
TEST(direct_method, test)
{
    LOG_TEST("------------------------------------direct method test-------------------------------------");

    cv::Mat img0 = cv::imread("../src/data/l0.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img1 = cv::imread("../src/data/l1.png", cv::IMREAD_GRAYSCALE);

    Eigen::Matrix3d K;
    K << 718.856, 0, 607.1928,
         0, 718.856, 185.2157,
         0, 0, 1;

    // // 显示
    // cv::Mat img_show(img0.rows * 2, img0.cols, CV_8UC1);
    // img0.copyTo(img_show(cv::Rect(0, 0, img0.cols, img0.rows)));
    // img1.copyTo(img_show(cv::Rect(0, img0.rows, img0.cols, img0.rows)));
    // cv::imshow("img_show", img_show);
    // cv::waitKey(0);
    
    DirectMethodOptimize direct_method_optimize;

    std::vector<Eigen::Vector2d> features = get_good_features(img0);

    LOG_TEST("features.size(): ", features.size());

    // TODO: 加入图像金字塔, 避免局部最优
    Eigen::MatrixXd measurements = Eigen::MatrixXd::Zero(features.size(), 2);
    for(uint i = 0; i < features.size(); ++i) measurements.row(i) = features[i];
    direct_method_optimize.Optimize(Eigen::VectorXd::Zero(6), measurements, K, img0, img1);

    direct_method_optimize.Print();
}

std::vector<Eigen::Vector2d> get_good_features(const cv::Mat& img)
{
    std::vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, 1000, 0.01, 10);
    std::vector<Eigen::Vector2d> features;
    for(auto& corner : corners)
        features.push_back(Eigen::Vector2d(corner.x, corner.y));
    return features;
}










// JUST_RUN_TEST(camera_pose, test)
TEST(camera_pose, test)
{
    LOG_TEST("------------------------------------camera_pose test-------------------------------------");
    // 像素坐标 -> 相机坐标 -> 世界坐标
    Eigen::Matrix3d K;
    K << 718.856, 0, 607.1928,
         0, 718.856, 185.2157,
         0, 0, 1;
    
    Eigen::Matrix3d R = Eigen::Matrix3d::Identity();
    Eigen::Vector3d t(0, 0, 0);

    Eigen::Vector2d p0_pixel(100, 100);
    Eigen::Vector3d p0 = K * Eigen::Vector3d(p0_pixel(0), p0_pixel(1), 1);
    Eigen::Vector3d p1 = R * p0 + t;
    Eigen::Vector3d p1_pixel = K.inverse() * p1;
    Eigen::Vector2d p1_pixel_norm = p1_pixel.head(2) / p1_pixel(2);

    LOG_TEST("p0_pixel: ", p0_pixel.transpose());
    LOG_TEST("p1_pixel: ", p1_pixel_norm.transpose());

}
