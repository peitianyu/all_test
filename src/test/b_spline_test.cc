#include "core/tt_test.h"
#include "common/b_spline.h"
#include <opencv2/viz.hpp>

// JUST_RUN_TEST(b_spline, test)
TEST(b_spline, test)
{
    BSpline<Eigen::Vector3f, float> bspline;

    // 一个正方体
    bspline.add_point(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    bspline.add_point(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
    bspline.add_point(Eigen::Vector3f(1.0f, 1.0f, 0.0f));
    bspline.add_point(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
    bspline.add_point(Eigen::Vector3f(0.0f, 0.0f, 1.0f));
    bspline.add_point(Eigen::Vector3f(1.0f, 0.0f, 1.0f));
    bspline.add_point(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
    bspline.add_point(Eigen::Vector3f(0.0f, 1.0f, 1.0f));

    bspline.uniform_knots(0.0f, 1.0f);
    // bspline.scale_knots(0.0f, 0.2f);

    std::vector<cv::Vec3f> viz_points;
    for (int i = 0; i < 1000; ++i) {
        float t = i / 1000.0f;
        Eigen::Vector3f p = bspline.evaluate(t);
        // std::cout << p.transpose() << std::endl;
        viz_points.push_back(cv::Vec3f(p.x(), p.y(), p.z()));
    }

    std::vector<cv::Vec3f> viz_control_points;
    for (auto const& p : bspline.get_points()) {
        viz_control_points.push_back(cv::Vec3f(p.x(), p.y(), p.z()));
    }

    cv::viz::Viz3d window("window");
    window.setBackgroundColor(cv::viz::Color::white());
    cv::viz::WCloud control_points_widget(viz_control_points, cv::viz::Color::red());
    control_points_widget.setRenderingProperty(cv::viz::POINT_SIZE, 5);
    cv::viz::WCloud cloud_widget(viz_points, cv::viz::Color::black());
    cloud_widget.setRenderingProperty(cv::viz::POINT_SIZE, 3);
    window.showWidget("cloud", cloud_widget);
    window.showWidget("control_points", control_points_widget);
    window.spin();
}