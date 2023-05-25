#include "core/tt_test.h"
#include "common/octree.h"

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
        octree.Insert(new Point3D(GetRandomPoint()));

    std::vector<Point3D> points = octree.GetPointsInsideSphere(Point3D::Zero(), 0.5);

    for (auto& p : points)
        ASSERT_LE(p.norm(), 0.5);
}
