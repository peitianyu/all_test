#include"core/tt_test.h"

void register_callback(void(*cb)(int));

static void (*g_cb)(int) = nullptr;
void register_callback(void(*cb)(int))
{
    g_cb = cb;
}

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