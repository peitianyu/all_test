#include "core/tt_test.h"
#include "core/tt_assert.h"
#include "common/octree.h"
#include <Eigen/Core>
#include <iostream>
#include <sstream>
#include <fstream>

#include <random>
#include <thread>
#include <pangolin/pangolin.h>
#include "core/tt_tictoc.h"
#include "common/pangolin_manage.h"

using Point = Eigen::Vector3d;
using Octr = OctTree<Point, double>;


void visual_octree(Octr& octree)
{
    pangolin::CreateWindowAndBind("Main",1920,1080);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Define Projection and initial ModelView matrix
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1920,1080,420,420,320,320,0.2,10000),
        pangolin::ModelViewLookAt(200,0,200, 0,0,0, pangolin::AxisY)
    );
    
    pangolin::Handler3D handler(s_cam); 
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -1920.0f/1080.0f)
            .SetHandler(&handler);

    while (true)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        d_cam.Activate(s_cam);
        
        auto print_node = [&](Octr::Node* node) {
            Eigen::Vector3d center = node->center;
            Eigen::Vector3d d_xyz = Eigen::Vector3d(64, 64, 64) / pow(2, node->depth+1);
        
            // 绘制实心立方体
            if(node->depth == 3){
                draw_color_cube(center[0], center[1], center[2], d_xyz[0], pangolin_set_color(node->depth));
            }

            glLineWidth(3);
            glBegin(GL_LINES);
            glColor3f(0.0f, 0.0f, 0.0f);
            draw_cuboid(center[0], center[1], center[2], d_xyz[0], d_xyz[1], d_xyz[2]);
            glEnd();
        };

        octree.visual(print_node); 

        glEnd();
        pangolin::FinishFrame();
    } 
}


// JUST_RUN_TEST(octree, test)
TEST(octree, test)
{
    std::string data_path = "../src/data/octree.txt";

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
            ss >> min(0) >> min(1) >> min(2) >> max(0) >> max(1) >> max(2);
        } else if (type == "obstacle") {
            double data;
            ss >> p[0] >> p[1] >> p[2] >> data;
            obstacles.push_back(p);
            datas.push_back(1); // 计数
            // datas.push_back(data);
        }
    }
    ifs.close();

    ///////////////////////construct//////////////////////////
    Octr octree(min, max, 4);
    for(size_t i = 0; i < obstacles.size(); ++i) {
        octree.insert(obstacles[i], datas[i]);
    }

    ///////////////////////find node//////////////////////////
    Octr::Node *node = octree.find(Point(14, 57, 24));
    std::cout << "find node: " << node->center.transpose() << " data: " << node->data << " depth: " << node->depth << std::endl;

    node = octree.find(Point(25, 25, 25), 1);
    std::cout << "find node: " << node->center.transpose() << " data: " << node->data << " depth: " << node->depth << std::endl;

    ///////////////////////visual//////////////////////////
    // visual_octree(octree);
}