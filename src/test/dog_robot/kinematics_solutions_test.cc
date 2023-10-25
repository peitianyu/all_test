#include"core/tt_test.h"
#include "common/log.h"

#include <cmath>

struct Pos
{
    float x;
    float y;
    float z;
};



struct Joint
{
    float alpha;    // 大腿
    float beta;     // 小腿
    float gamma;    // 胯关节
};

struct Param
{
    float la; // 大腿长度
    float lb; // 小腿长度

    float w;  // 身体宽度
    float l;  // 身体长度

    // 斜边与长边夹角
    float theta() const { return atan2(w, l); }
    float L() const { return sqrt(w * w + l * l)/2; }
};

// https://imuncle.github.io/content.html?id=61
/****************************运动学逆解算*******************************/
Joint inverse_solution(const Pos& pos, const Param& param)
{
    float x2 = pos.x * pos.x;
    float y2 = pos.y * pos.y;
    float z2 = pos.z * pos.z;
    float la2 = param.la * param.la;
    float lb2 = param.lb * param.lb;
    float x2y2z2 = x2 + y2 + z2;

    float alpha = M_PI - atan2(sqrt(x2 + z2), pos.x) - acos((la2 + x2y2z2 - lb2) / (2 * param.la * sqrt(x2y2z2)));
    float beta = acos((la2 + lb2 - x2y2z2) / (2 * param.la * param.lb));
    float gamma = atan2(pos.y, pos.x);

    return (Joint){alpha, beta, gamma};
}

struct Euler
{
    float yaw;
    float roll;
    float pitch;
};

// https://imuncle.github.io/content.html?id=62
/************************************姿态逆解算******************************************/
Pos pos_inverse(const Euler& euler, const Param& param)
{
    // yaw
    float dx_yaw = param.w/2 - param.L() * sin(param.theta() - euler.yaw);
    float dy_yaw = param.l/2 - param.L() * cos(param.theta() - euler.yaw);

    // pitch
    float dx_pitch = param.l/2 * (1- cos(euler.pitch));
    float dy_pitch = param.l/2 * sin(euler.pitch);

    // roll
    float dx_roll = param.w/2 * (1 - cos(euler.roll));
    float dy_roll = param.w/2 * sin(euler.roll);

    return (Pos){dx_yaw + dx_pitch + dx_roll, dy_yaw + dy_pitch + dy_roll, 0};
}



JUST_RUN_TEST(kinematics_solutions, test)
TEST(kinematics_solutions, test)
{
    // LOG_TEST("kinematics_solutions", "test");
    Pos pos = {0.2, 0.0, 0.1};
    Param param = {0.1, 0.3, 0.2, 0.4};

    Joint joint = inverse_solution(pos, param);
    LOG_TEST("alpha: ", joint.alpha);
    LOG_TEST("beta: ", joint.beta);
    LOG_TEST("gamma: ", joint.gamma);

    Euler euler = {1.0, 0.1, 0.0};
    Pos pos1 = pos_inverse(euler, param);

    LOG_TEST("pos1.x: ", pos1.x);
    LOG_TEST("pos1.y: ", pos1.y);
}
