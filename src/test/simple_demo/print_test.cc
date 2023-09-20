#include"core/tt_test.h"

template <typename... Args>
std::ostream & print0(Args&&... args) {
    auto a = {(std::cout << std::forward<Args>(args) << " ", 0)...};
    (void)a;
    return std::cout;
}

// JUST_RUN_TEST(print, test)
TEST(print, test)
{
    print0("");
    print0(1, 2, 3, "4", 5, 6.5, 7, 8, 9) << std::endl;
}


template <typename T>
std::ostream &print(std::ostream &os, const T &arg) { return os << arg;}
   
template <typename T, typename... Types>
std::ostream &print(std::ostream &os, const T &firstArg, const Types &...args){ 
    os << firstArg << " ";
    return print(os, args...);
}

// JUST_RUN_TEST(print1, test)
TEST(print1, test)
{
    print(std::cout, 1, 2, 3, "4", 5, 6.5, 7, 8, 9) << std::endl;
}


// JUST_RUN_TEST(print2, test)
TEST(print2, test)
{
    float f = 1.f;
    printf("%f\n", f);
}
