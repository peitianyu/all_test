#include "core/tt_test.h"
#include "core/tt_assert.h"
#include "common/octree.h"
#include "3dr_party/plot.h"
#include "3dr_party/plot_manage.h"
#include <Eigen/Core>
#include <iostream>
#include <sstream>
#include <fstream>

using Point = Eigen::Vector2d;
using Quad = QuadTree<Point, double>;


JUST_RUN_TEST(quadtree, test)
TEST(quadtree, test)
{
    std::string data_path = "../src/data/quadtree.txt";

    Point min = Point::Zero();
    Point max = Point::Zero();
    std::vector<Point> obstacles;
    std::vector<double> datas;

    std::ifstream ifs(data_path);
    if (!ifs.is_open()) {
        std::cout << "open file failed: " << data_path << std::endl;
        return;
    }
    std::string line;
    while(!ifs.eof()) {
        std::getline(ifs, line);
        std::stringstream ss(line);
        Point p = Point::Zero();
        std::string type;
        ss >> type;
        if (type == "boundary") {
            ss >> min(0) >> min(1) >> max(0) >> max(1);
        } else if (type == "obstacle") {
            double data;
            ss >> p[0] >> p[1] >> data;
            obstacles.push_back(p);
            datas.push_back(1); // 计数
            // datas.push_back(data);
        }
    }
    ifs.close();

    ///////////////////////construct//////////////////////////
    Quad quadtree(min, max, 4);
    for(size_t i = 0; i < obstacles.size(); ++i) {
        quadtree.insert(obstacles[i], datas[i]);
    }

    ///////////////////////find node//////////////////////////
    Quad::Node *node = quadtree.find(Point(50, 25));
    std::cout << "find node: " << node->center.transpose() << " data: " << node->data << " depth: " << node->depth << std::endl;

    node = quadtree.find(Point(25, 25), 1);
    std::cout << "find node: " << node->center.transpose() << " data: " << node->data << " depth: " << node->depth << std::endl;

    ///////////////////////visual//////////////////////////
    std::function<void(Quad::Node* node)> print_node = [](Quad::Node* node) {
        std::cout << "node: " << node->center.transpose() << " data: " << node->data << " depth: " << node->depth << std::endl;
    };
    quadtree.visual(print_node);

    // 绘制
    PlotManager plot_manager(100, 100);
    std::function<void(Quad::Node* node)> plot_node = [&](Quad::Node* node) {
        Quad::Boundary boundary;
        quadtree.find_boundary(node, boundary);
        auto& line = plot_manager.draw_rect(boundary.min(0), boundary.min(1), boundary.max(0) - boundary.min(0), 
                                            boundary.max(1) - boundary.min(1), node->depth);
        plot_manager.draw_marker(line, node->center(0), node->center(1)).label(std::to_string(int(node->data)), 0, 3); 
    };
    quadtree.visual(plot_node);
    plot_manager.write("quadtree.svg");

    ///////////////////////remove//////////////////////////
    quadtree.remove(Point(50, 25), 1);
    plot_manager.reset(100, 100);
    quadtree.visual(plot_node);
    plot_manager.write("quadtree_remove.svg");
}




