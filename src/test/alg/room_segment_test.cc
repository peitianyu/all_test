#include "core/tt_test.h"
#include "common/log.h"
#include "common/room_segment.h"
#include <iostream>

// JUST_RUN_TEST(room_segment, test)
TEST(room_segment, test)
{
    LOG_TEST("------------------------------room_segment--------------------------");

    RoomSegment room_segment({0.03, 30, 1600, 10, 60, "../src/data/Freiburg79_scan.png", "./"});
    room_segment.run();
}
