#include"core/tt_test.h"

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

// JUST_RUN_TEST(demo, test2)
TEST(demo, test2)
{
    bool is_false = false;
    ASSERT_TRUE(is_false);
    ASSERT_FALSE(!is_false);

    int a = 1;
    int b = 2;
    ASSERT_EQ(a, b);
}