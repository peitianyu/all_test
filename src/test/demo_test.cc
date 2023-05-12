#include"common/tt_test.h"

JUST_RUN_TEST(demo, test)
TEST(demo, test)
{
    ASSERT_TRUE(1 == 1);
    ASSERT_FALSE(1 == 2);
    ASSERT_EQ(1, 1);
}

// 打开只运行demo test2测试文件, 若关闭则所有测试文件全部运行
TEST(demo, test1)
{
    int *p = nullptr;
    *p = 1;
}