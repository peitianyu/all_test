#include"core/tt_test.h"

#define __used  __attribute__((__used__)) // 用于消除编译器警告

#define __constructor__  __attribute__((__constructor__)) // 用于函数，表示在main函数之前执行
__constructor__ void test_constructor()
{
    printf("test_constructor\n");
}

#define __section__(name)  __attribute__((__section__(name))) // 用于变量，表示将变量放在指定的section中
void test_section() __section__("test_section");

void test_section()
{
    printf("test_section\n");
}

// JUST_RUN_TEST(attribute, test_constructor)
TEST(attribute, test_constructor)
{
    printf("attribute test_constructor\n");

    test_section();
}

