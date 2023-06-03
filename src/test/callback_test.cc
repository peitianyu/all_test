#include"core/tt_test.h"

void register_callback(void(*cb)(int));

// 注册
static void (*g_cb)(int) = nullptr;
void register_callback(void(*cb)(int))
{
    g_cb = cb;
}

// 调用
void callback(int id)
{
    std::cout << "callback: " << id << std::endl;
}

// JUST_RUN_TEST(callback, test)
TEST(callback, test)
{
    register_callback(callback);

    g_cb(1);
    g_cb(2);
    g_cb(3);
}


class A
{
public:
    void callback(int id)
    {
        std::cout << "A::callback: " << id << std::endl;
    }
};

void register_callback(void(A::*cb)(int), A* a, int id);

// 注册
static void (A::*g_cb2)(int) = nullptr;
static A* g_a = nullptr;
void register_callback(void(A::*cb)(int), A* a, int id)
{
    g_cb2 = cb;
    g_a = a;
}

// 调用
void callback2(int id)
{
    (g_a->*g_cb2)(id);
}

// JUST_RUN_TEST(callback, test2)
TEST(callback, test2)
{
    A a;
    register_callback(&A::callback, &a, 1);

    callback2(1);
    callback2(2);
    callback2(3);
}