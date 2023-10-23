#include"core/tt_test.h"
#include "common/log.h"
#include "common/keybord_read.h"

JUST_RUN_TEST(keybord_read, test)
TEST(keybord_read, test)
{
    std::vector<int> datas;
    for(int i = 0; i < 10000; i++) datas.push_back(i);

    KeybordRead keybord_read(200);

    while (true)
    {
        int data = keybord_read.get_data(datas);
        LOG_TEST("keybord_read", "data: ", data);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

