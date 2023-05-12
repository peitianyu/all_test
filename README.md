## 测试单元模板, 实现TEST与段错误检测

### 测试在src/test中新建test文件, 例如src/test/demo_test.cc

```c++
#include"common/tt_test.h"

TEST(demo, test)
{
    ASSERT_TRUE(1 == 1);
    ASSERT_FALSE(1 == 2);
    ASSERT_EQ(1, 1);
}

// 打开只运行demo test2测试文件, 若关闭则所有测试文件全部运行
JUST_RUN_TEST(demo, test1)
TEST(demo, test1)
{
    int *p = nullptr;
    *p = 1;
}
```

### 输出结果

[![p9sb676.png](https://s1.ax1x.com/2023/05/12/p9sb676.png)](https://imgse.com/i/p9sb676)
