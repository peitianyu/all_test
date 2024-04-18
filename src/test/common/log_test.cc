#include "core/tt_test.h"
#include "common/log.h"
#include <iostream>

// JUST_RUN_TEST(log, test)
TEST(log, test)
{
    LOG_TEST("------------------------------log--------------------------");
}

#include <fstream>

// JUST_RUN_TEST(log1, test)
TEST(log1, test)
{
    LOG(TT_PRINT_RED, "[ERROR]", CODE_INFO, "hello world");

    std::string log_path = "log.txt";
    std::ofstream out(log_path, std::ios::app);
    LOG_FILE(out, "[ERROR]", "hello world");
    out.close();

    std::cout << "nihao" << std::endl;
    std::cerr << "nihao" << std::endl;
}
