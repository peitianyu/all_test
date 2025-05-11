#include "core/tt_test.h"
#include "common/log.h"
#include "common/ini_parse.h"


// JUST_RUN_TEST(ini_parse, test)
TEST(ini_parse, test)
{
    LOG_TEST("------------------------------ini_parse--------------------------");

    IniParse ini_parse("../src/data/test.ini");
    std::string str;
    if(!ini_parse.GetValue("section1", "str", str)) {
        LOG_TEST("get value failed");
    }
    int a = 0;
    if(!ini_parse.GetValue("section1", "a", a)) {
        LOG_TEST("get value failed");
    }
    float b = 0;
    if(!ini_parse.GetValue("section1", "b", b)) {
        LOG_TEST("get value failed");
    }

    LOG_TEST("str = ", str);
    LOG_TEST("a = ", a);
    LOG_TEST("b = ", b);
}
