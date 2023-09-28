#include "core/tt_test.h"
#include "common/a_star.h"
#include "3dr_party/plot_manage.h"


JUST_RUN_TEST(a_star, test)
TEST(a_star, test)
{
    path_plan::Astar astar("../src/data/a_star.grid");

    std::vector<path_plan::Point> path = astar.find_path(path_plan::Point(0, 0), path_plan::Point(19, 0));

    // std::cout << "path: " << std::endl;
    // for (auto &p : path) {
    //     std::cout << p.x << ", " << p.y << std::endl;
    // }

    /////////////////////////plot//////////////////////////
    int width, height;
    astar.get_size(width, height);
    uint scale = 10;
    PlotManager plot(width*scale, height*scale);

    enum MODE
    {
        GRID = 0,
        OBSTACLE = 1,
        PATH = 2,
        DYNAMIC_PATH = 3,
    };

    // 绘制栅格
    plot.draw_grid(scale, GRID);

    // 绘制障碍物
    for (int i = 0; i < width; ++i) 
    for (int j = 0; j < height; ++j) 
        if (astar.is_occ(i, j)) plot.draw_rect(i*scale, j*scale, scale, scale, OBSTACLE).drawFill(); 
           
    // 绘制路径
    for(uint i = 0; i < path.size(); ++i) 
        plot.draw_rect(path[i].x*scale, path[i].y*scale, scale, scale, PATH).drawFill();

    // 绘制动态路径
    auto& line = plot.line(DYNAMIC_PATH);
    for(uint i = 0; i < path.size(); ++i) 
        plot.draw_rect(line, path[i].x*scale, path[i].y*scale, scale, scale).drawFill().toFrame(0.5*i);

    plot.show_legend(line, "label", 2, 1);
    plot.write("a_star.svg");
}