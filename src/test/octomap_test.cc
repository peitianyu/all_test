#include "core/tt_test.h"
#include "common/octomap.h"

Point3D GetRandomPoint1()
{
    return Point3D::Random();
}


// JUST_RUN_TEST(octomap, test)
TEST(octomap, test)
{
    OctoMap::Option option;
    OctoMap octo_map(option);

    for (int i = 0; i < 20; ++i)
        octo_map.InsertPoint(GetRandomPoint1(), rand() % 2);

    octo_map.Traverse();
}

