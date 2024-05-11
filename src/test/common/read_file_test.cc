#include "core/tt_test.h"
#include "common/read_file.h"



// JUST_RUN_TEST(read_file, test)
TEST(read_file, test)
{
    ReadFile rf("../src/data/read_file.txt");
    std::cout << rf.get_line() << std::endl;

    std::string str;
    int a;
    rf.get_stringstream() >> str >> a;
    std::cout << str << " " << a << std::endl;
}