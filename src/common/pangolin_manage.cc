#include "pangolin_manage.h"

void draw_line(float x1, float y1, float z1, float x2, float y2, float z2) {
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
};

void draw_cuboid(float x, float y, float z, float dx, float dy, float dz) {
    draw_line(x - dx, y - dy, z - dz, x - dx, y - dy, z + dz);
    draw_line(x - dx, y - dy, z - dz, x - dx, y + dy, z - dz);
    draw_line(x - dx, y - dy, z - dz, x + dx, y - dy, z - dz);
    draw_line(x - dx, y - dy, z + dz, x - dx, y + dy, z + dz);
    draw_line(x - dx, y - dy, z + dz, x + dx, y - dy, z + dz);
    draw_line(x - dx, y + dy, z - dz, x - dx, y + dy, z + dz);
    draw_line(x - dx, y + dy, z - dz, x + dx, y + dy, z - dz);
    draw_line(x - dx, y + dy, z + dz, x + dx, y + dy, z + dz);
    draw_line(x + dx, y - dy, z - dz, x + dx, y - dy, z + dz);
    draw_line(x + dx, y - dy, z - dz, x + dx, y + dy, z - dz);
    draw_line(x + dx, y - dy, z + dz, x + dx, y + dy, z + dz);
    draw_line(x + dx, y + dy, z - dz, x + dx, y + dy, z + dz);
};

void draw_cube(float x, float y, float z, float r) {
    draw_cuboid(x, y, z, r, r, r);
};

Eigen::Vector3f pangolin_set_color(const uint& depth)
{
    switch (depth)
    {
    case 0:
        glColor3f(1.0f, 0.0f, 0.0f);
        return Eigen::Vector3f(1.0f, 0.0f, 0.0f);
    case 1:
        glColor3f(0.0f, 1.0f, 0.0f);
        return Eigen::Vector3f(0.0f, 1.0f, 0.0f);
    case 2:
        glColor3f(0.0f, 0.0f, 1.0f);
        return Eigen::Vector3f(0.0f, 0.0f, 1.0f);
    case 3:
        glColor3f(1.0f, 1.0f, 0.0f);
        return Eigen::Vector3f(1.0f, 1.0f, 0.0f);
    case 4:
        glColor3f(1.0f, 0.0f, 1.0f);
        return Eigen::Vector3f(1.0f, 0.0f, 1.0f);
    case 5:
        glColor3f(0.0f, 1.0f, 1.0f);
        return Eigen::Vector3f(0.0f, 1.0f, 1.0f);
    case 6:
        glColor3f(1.0f, 1.0f, 1.0f);
        return Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    case 7:
        glColor3f(0.0f, 0.0f, 0.0f);
        return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    default:
        glColor3f(1.0f, 1.0f, 1.0f);
        return Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    }
}

void draw_color_cube(float x, float y, float z, float r, const Eigen::Vector3f& color) {

    const GLfloat verts[] = {
        x-r, y-r, z+r,  x+r, y-r, z+r,  x-r, y+r, z+r,  x+r, y+r, z+r,  // FRONT
        x-r, y-r, z-r,  x-r, y+r, z-r,  x+r, y-r, z-r,  x+r, y+r, z-r,  // BACK
        x-r, y-r, z+r,  x-r, y+r, z+r,  x-r, y-r, z-r,  x-r, y+r, z-r,  // LEFT
        x+r, y-r, z-r,  x+r, y+r, z-r,  x+r, y-r, z+r,  x+r, y+r, z+r,  // RIGHT
        x-r, y+r, z+r,  x+r, y+r, z+r,  x-r, y+r, z-r,  x+r, y+r, z-r,  // TOP
        x-r, y-r, z+r,  x-r, y-r, z-r,  x+r, y-r, z+r,  x+r, y-r, z-r   // BOTTOM
    };

    glVertexPointer(3, GL_FLOAT, 0, verts);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor4f(color[0], color[1], color[2], 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
};

