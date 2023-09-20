#include"core/tt_test.h"
#include"common/deprecated.h"

DEPRECATED("已弃用")
void deprecated_func()
{
    std::cout << "deprecated_func" << std::endl;
}

TEST(Deprecated, test)
{
    // deprecated_func();
}