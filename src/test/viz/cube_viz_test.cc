#include "core/tt_test.h"
#include <opencv2/viz.hpp>

// JUST_RUN_TEST(cube_viz, cube_viz_test)
TEST(cube_viz, cube_viz_test)
{
    cv::viz::Viz3d viz("cube");
    viz.setBackgroundColor(cv::viz::Color::white());

    viz.showWidget("Cube Widget", cv::viz::WCube(cv::Point3f(0.5, 0.5, 0.0), cv::Point3f(0.0, 0.0, -0.5), true, cv::viz::Color::black()));
    
    float prob = 0.5;
    viz.showWidget("Cube Widget", cv::viz::WCube(cv::Point3f(0.5, 0.5, 0.0), cv::Point3f(0.0, 0.0, -0.5), false, cv::viz::Color(0, uint(255 * (1 - prob)), 0)));
    
    viz.spin();
}