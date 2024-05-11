#include"core/tt_test.h" 

class TestSingleton
{
public:
    static TestSingleton &GetInstance()
    {
        static TestSingleton instance;
        return instance;
    }

    TestSingleton(const TestSingleton &) = delete;
    TestSingleton &operator=(const TestSingleton &) = delete;

    void Print()
    {
        std::cout << "TestSingleton::Print()" << std::endl;
    }
private:
    TestSingleton() = default;
    ~TestSingleton() = default;
};


// JUST_RUN_TEST(singleton, test)
TEST(singleton, test)
{
    TestSingleton::GetInstance().Print();
}

