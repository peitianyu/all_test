#include "core/tt_test.h"
#include "core/tt_dbg.h"

int32_t factorial(int32_t n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

struct Point
{
    float x;
    float y;

    friend std::ostream& operator<<(std::ostream& os, const Point& p)
    {
        os << "(" << p.x << ", " << p.y << ")";
        return os;
    }
};

// JUST_RUN_TEST(dbg, test)
TEST(dbg, test)
{
    std::string msg = "hello world";
    dbg(msg);

    int32_t a = 2;
    dbg(3 * a);

    uint a1 = 2;
    dbg(3 * a1);

    std::vector<int32_t> numbers{1, 2, 3};
    dbg_vector(numbers);

    dbg("this line is executed");

    dbg(factorial(5));

    Point p{1.0f, 2.0f};
    dbg(p);
}