#include"core/tt_test.h"
#include "common/log.h"

#include <cmath>

struct Pos
{
    float x, y, z;
};

struct Control
{
    float alfa, beta, gamma;
};

Control pos_inverse(const Pos& pos)
{
    float h = 45.0;
    float hu = 125.5899;
    float hl = 136.0;

    float dyz = sqrt(pos.y*pos.y+pos.z*pos.z);
    float lyz = sqrt(dyz*dyz-h*h);

    LOG_TEST("pos_inverse", "dyz: ", dyz);
    LOG_TEST("pos_inverse", "lyz: ", lyz);


    float gamma_yz = -atan(pos.y/pos.z);
    float gamma_h_offset = -atan(h/lyz);
    float gamma = gamma_yz-gamma_h_offset;//gamma理解为上图的T

    float lxzp = sqrt(lyz*lyz+pos.x*pos.x);
    float n = (lxzp*lxzp-hl*hl-hu*hu)/(2*hu);
    float beta = -acos(n/hl); //β

    float alfa_xzp = -atan(pos.x/lyz);
    float alfa_off = acos((hu+n)/lxzp);
    float alfa = (alfa_xzp+alfa_off); //α

    return Control{alfa, beta, gamma};
}

// JUST_RUN_TEST(pos_inverse, test)
TEST(pos_inverse, test)
{
    LOG_TEST("pos_inverse", "test");

    Pos pos{20, 20, 45};
    Control control = pos_inverse(pos);
    std::cout << "alfa: " << control.alfa << std::endl;
    std::cout << "beta: " << control.beta << std::endl;
    std::cout << "gamma: " << control.gamma << std::endl;
}
