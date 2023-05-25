#include "core/tt_test.h"
#include "common/log.h"



// JUST_RUN_TEST(log, test)
TEST(log, test)
{
    LOG_INFO("hello world");
    LOG_DEBUG("hello world");
    LOG_WARN("hello world");
    LOG_ERROR("hello world");
}