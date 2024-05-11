#include "core/tt_test.h"
#include <iostream>
#include <memory>

class PimplTest {
public:
    PimplTest() : pimpl_(std::make_unique<Pimpl>()) {}

    void DoSomething();
private:
    class Pimpl;
    std::unique_ptr<Pimpl> pimpl_;
};

class PimplTest::Pimpl {
public:
    void DoSomething() { printf("DoSomething\n"); }
};

void PimplTest::DoSomething() { pimpl_->DoSomething(); }

// JUST_RUN_TEST(pimpl, test)
TEST(pimpl, test)
{
    PimplTest pimpl_test;
    pimpl_test.DoSomething();
}