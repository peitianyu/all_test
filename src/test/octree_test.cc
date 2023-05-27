#include "core/tt_test.h"
#include "common/octree.h"
#include "core/tt_assert.h"
// Random number between [-1,1]
Point3D GetRandomPoint()
{
    return Point3D::Random();
}

JUST_RUN_TEST(octree, test)
TEST(octree, test)
{
    Octree octree(Point3D::Zero(), Point3D(1, 1, 1));

    for (int i = 0; i < 200; ++i)
        octree.Insert(GetRandomPoint());

    std::vector<Point3D> points = octree.GetPointsInsideSphere(Point3D::Zero(), 0.5);

    for (auto& p : points)
        EXPECT_LE(p.norm(), 0.5);
}
