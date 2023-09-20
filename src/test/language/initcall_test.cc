#include"core/tt_test.h"

typedef void (*initcall_t)(void);
#define __used  __attribute__((__used__)) // 用于消除编译器警告

#define __define_initcall(fn, id) \
    static const initcall_t __initcall_##fn##id __used \
    __attribute__((__section__("initcall" #id "init"))) = fn;
#define __do_initcall(fn) do {  if (fn) (*fn)(); } while (0)

#define pure_initcall(fn)       __define_initcall(fn, 0) //可用作系统时钟初始化
#define fs_initcall(fn)         __define_initcall(fn, 1) //tick和调试接口初始化

#define do_initcall() \
    do { \
        extern initcall_t __start_initcall0init[], __stop_initcall0init[]; \
        initcall_t *call; \
        for (call = __start_initcall0init; call < __stop_initcall0init; call++) \
            __do_initcall(*call); \
        extern initcall_t __start_initcall1init[], __stop_initcall1init[]; \
        for (call = __start_initcall1init; call < __stop_initcall1init; call++) \
            __do_initcall(*call); \
    } while (0) 

void test1(){ printf("test1\n"); }
pure_initcall(test1)

void test2() { printf("test2\n"); }
fs_initcall(test2)

// JUST_RUN_TEST(initcall, test)
TEST(initcall, test)
{
    printf("initcall test\n");

    do_initcall();
}

