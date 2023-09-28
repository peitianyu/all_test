#include "core/tt_test.h"
#include "3dr_party/plot_manage.h"
#include <random>

JUST_RUN_TEST(plot_manage, default)
TEST(plot_manage, default)
{
    PlotManager plot(100, 100);
    plot.draw_line(0, 0, 1, 1, 0);
    auto& line = plot.draw_rect(0, 0, 1, 1, 1);
    plot.draw_circle(0, 0, 1, 2);


    for(uint i = 0; i < 5; ++i) {
        plot.draw_marker(line, i, i).toFrame(i);
    }

    plot.show_legend(line, "label", 2, 1);
    plot.write("plot_manage.svg");
}
