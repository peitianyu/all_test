#include"core/tt_test.h"



template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
    void implementation() {
        std::cout << "Base::implementation()" << std::endl;
    }
};


class Derived1 : public Base<Derived1> {
public:
    void implementation() {
        std::cout << "Derived1::implementation()" << std::endl;
    }
};


class Derived2 : public Base<Derived2> {
public:
    void implementation() {
        std::cout << "Derived2::implementation()" << std::endl;
    }
};


// JUST_RUN_TEST(crtp, test)
TEST(crtp, test)
{
    Derived1 d1;
    d1.interface();
    Derived2 d2;
    d2.interface();
}