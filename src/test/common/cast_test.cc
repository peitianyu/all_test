#include"core/tt_test.h"


// 测试四种cast
// JUST_RUN_TEST(cast, test)
TEST(cast, test)
{
    // const_cast
    const int* p = new int(10);
    int* p1 = const_cast<int*>(p);
    p1[0] = 20; // 通过const_cast去掉const属性后，可以修改指针指向的值
    ASSERT_EQ(p1[0], 20);

    // static_cast
    int a = 10;
    double b = static_cast<double>(a);
    ASSERT_EQ(typeid(b), typeid(double));

    // reinterpret_cast
    int* p2 = new int(10);
    int64_t p3 = reinterpret_cast<int64_t>(p2);
    ASSERT_EQ(typeid(p3), typeid(int64_t));
}