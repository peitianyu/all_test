#ifndef __PANGOLIN_MANAGE_H__
#define __PANGOLIN_MANAGE_H__

#include <pangolin/pangolin.h>
#include <Eigen/Core>

#define PANGOLINE_BLUE      Eigen::Vector3f(0.0f, 0.0f, 1.0f)
#define PANGOLINE_RED       Eigen::Vector3f(1.0f, 0.0f, 0.0f)
#define PANGOLINE_GREEN     Eigen::Vector3f(0.0f, 1.0f, 0.0f)
#define PANGOLINE_YELLOW    Eigen::Vector3f(1.0f, 1.0f, 0.0f)
#define PANGOLINE_PURPLE    Eigen::Vector3f(1.0f, 0.0f, 1.0f)
#define PANGOLINE_CYAN      Eigen::Vector3f(0.0f, 1.0f, 1.0f)
#define PANGOLINE_WHITE     Eigen::Vector3f(1.0f, 1.0f, 1.0f)
#define PANGOLINE_BLACK     Eigen::Vector3f(0.0f, 0.0f, 0.0f)

void draw_line(float x1, float y1, float z1, float x2, float y2, float z2);

void draw_cuboid(float x, float y, float z, float dx, float dy, float dz);
void draw_cube(float x, float y, float z, float r);
Eigen::Vector3f pangolin_set_color(const uint& depth);

void draw_color_cube(float x, float y, float z, float r, const Eigen::Vector3f& color);



#endif // __PANGOLIN_MANAGE_H__