## 测试单元模板, 实现TEST与段错误检测

### 测试在src/test中新建test文件, 例如src/test/demo_test.cc

```c++
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
```

### 输出结果

[![p9yVUqx.png](https://s1.ax1x.com/2023/05/12/p9yVUqx.png)](https://imgse.com/i/p9yVUqx)

### !注意

```txt
1. 使用多个JUST_RUN_TEST时会编译不通过, 会报: multiple definition of `g_justtest_Test'
```
