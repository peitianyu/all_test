#include "core/tt_test.h"
#include "3dr_party/plot.h"
#include <random>

// JUST_RUN_TEST(plot2d, default)
TEST(plot2d, default)
{
    signalsmith::plot::Plot2D plot;

    // Customise the axes
    plot.x.major(0).tick(10).label("time");
    plot.y.major(0).minors(-1, 1).label("signal");

    // Add some data with `.add(x, y)`
    auto &sin = plot.line(), &cos = plot.line();
    for (double x = 0; x < 10; x += 0.01) {
        sin.add(x, std::sin(x));
        cos.add(x, std::cos(x));
    }
    sin.label("sin(x)");
    cos.label("cos(x)");

    plot.write("out/default-2d.svg");
}

// JUST_RUN_TEST(plot2d, circles)
TEST(plot2d, circles)
{
    signalsmith::plot::Plot2D plot(200, 200);
    // No ticks or grid
    plot.x.blank();
    plot.y.blank();
    auto &legend = plot.legend(2, 1); // 标签放置到位置
    auto &line = plot.fill();
    std::mt19937 rng(0);
    for(double t = 0; t < 100.0; t += 1) {
        std::uniform_real_distribution<> dist(-1, 1);
        for (int i = 0; i < 100; ++i) {
            line.marker(dist(rng), dist(rng));
        }
        line.toFrame(t);
    }
    line.loopFrame(100);
    legend.add(line, "circles");
    plot.write("filled-circles.svg");
}

// JUST_RUN_TEST(plot2d, styles)
TEST(plot2d, styles)
{
    signalsmith::plot::Plot2D plot(100, 100);
		
    // Prevent it from adding default ticks
    plot.x.linear(-1, 1).major(0).minors(-1, 1);
    plot.y.copyFrom(plot.x);

    auto &line = plot.line(3);  // 指定style_mode
    line.marker(0, 0).label("label", 0, 10);

    // Add a line
    auto &line1 = plot.line(1);	
    line1.marker(-0.5, -0.5).label("line1", 0, 10);
    
    // Remove bottom ticks
    plot.write("style-sequence.svg");
}

static void draw_rec(double x_min, double x_max, double y_min, double y_max, signalsmith::plot::Plot2D &plot)
{
    std::vector<double> x = {x_min, x_max, x_max, x_min, x_min};
    std::vector<double> y = {y_min, y_min, y_max, y_max, y_min};
    auto &line = plot.line();
    line.addArray(x, y);
}


// JUST_RUN_TEST(plot2d, rec)
TEST(plot2d, rec)
{
    // 绘制矩形(0.0, 0.0)(50, 50)
    signalsmith::plot::Plot2D plot(100, 100);
    draw_rec(0, 50, 0, 50, plot);

    // 绘制矩形(10.0, 10.0)(30, 30)
    draw_rec(10, 30, 10, 30, plot);

    // 绘制矩形(20.0, 20.0)(40, 40)
    draw_rec(20, 40, 20, 40, plot);
    plot.write("rec.svg");
}
