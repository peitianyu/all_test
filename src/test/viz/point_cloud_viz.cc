#include "core/tt_test.h"
#include "common/point_cloud_visual.h"

// JUST_RUN_TEST(point_cloud_visual, test)
TEST(point_cloud_visual, test)
{
    PointCloudVisual point_cloud_visual;
    point_cloud_visual("../src/data/ground.txt");
}