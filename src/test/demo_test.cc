#include"common/tt_test.h"

// JUST_RUN_TEST(demo, test)
TEST(demo, test)
{
    ASSERT_TRUE(1 == 1);
    ASSERT_FALSE(1 == 2);
    ASSERT_EQ(1, 1);
}


SKIP_TEST(demo, test1)
// TEST(demo, test1)
{
    int *p = nullptr;
    *p = 1;
}

TEST(demo, test2)
{
    ASSERT_EQ(1, 2);
}