#include "core/tt_test.h"
#include "common/timer.h"
#include "core/tt_tictoc.h"

// JUST_RUN_TEST(timer, test)
TEST(timer, test)
{
    TicToc tic_toc;

    uint period = 1e3; // ms
    Timer timer(period);
    
    while (true) {
        if (timer.is_alarm()) { 
            timer.set_period(period += 1e3);
            timer.reset();

            std::cout << "time: " << tic_toc.Toc() << std::endl;
            tic_toc.Tic();
        }
    }
}
