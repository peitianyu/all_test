#include "core/tt_test.h"
#include <iostream>
#include <memory>

class EmptyBase {};

class Derived : public EmptyBase
{
public:
    Derived() : i_(0) {}
    Derived(int i) : i_(i) {}
    int i_;
};

// JUST_RUN_TEST(ebco, test)
TEST(ebco, test)
{
    std::cout << "sizeof(EmptyBase): " << sizeof(EmptyBase) << std::endl;
    std::cout << "sizeof(Derived): " << sizeof(Derived) << std::endl;
    std::cout << "sizeof(std::unique_ptr<EmptyBase>): " << sizeof(std::unique_ptr<EmptyBase>) << std::endl;
    std::cout << "sizeof(std::unique_ptr<Derived>): " << sizeof(std::unique_ptr<Derived>) << std::endl;
    std::cout << "sizeof(std::shared_ptr<EmptyBase>): " << sizeof(std::shared_ptr<EmptyBase>) << std::endl;
    std::cout << "sizeof(std::shared_ptr<Derived>): " << sizeof(std::shared_ptr<Derived>) << std::endl;
}