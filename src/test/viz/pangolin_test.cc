#include "core/tt_test.h"
#include <pangolin/pangolin.h>
#include "core/tt_tictoc.h"

#include <random>
#include <thread>
#include "common/pangolin_manage.h"


// JUST_RUN_TEST(pangolin, default)
TEST(pangolin, default)
{
    pangolin::CreateWindowAndBind("Main",1920,1080);
    glEnable(GL_DEPTH_TEST);

    // Define Projection and initial ModelView matrix
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1920,1080,800,500,960,540,0.2,100),
        pangolin::ModelViewLookAt(-2,2,-2, 0,0,0, pangolin::AxisY)
    );

    // Create Interactive View in window
    pangolin::Handler3D handler(s_cam);
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -1920.0f/1080.0f)
            .SetHandler(&handler);

    TicToc tictoc;
    while(1)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        d_cam.Activate(s_cam);

        draw_color_cube(0, 0, 0, 1, Eigen::Vector3f(1.0, 0.0, 0.0));

        glLineWidth(2);
        glBegin(GL_LINES);

        static float r = 1.0;
        static float c = 1.0;
        glColor3f(c, c, c);
        // draw_cube(0, 0, 0, r);
        
        draw_cuboid(0, 0, 0, 1, 2, 3);

        
        if(tictoc.Toc() > 0.5) {
            r += 0.1;
            c*=0.9;

            tictoc.Tic();
        }

        glEnd();
        pangolin::FinishFrame();
    }
}