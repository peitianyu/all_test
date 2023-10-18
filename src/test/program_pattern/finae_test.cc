#include"core/tt_test.h"


#include <iostream>
#include <type_traits>

template <typename T>
class has_serialize
{
    // 使用 SFINAE 技术检查类型 T 是否具有 serialize 成员函数
    template <typename U>
    static auto test(int) -> decltype(std::declval<U>().serialize(), std::true_type());

    template <typename U>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

class A { public: void serialize() {} };
class B { public: void serialize(int) {} };
class C { public: int serialize() { return 0; } };
class D { private: int serialize() { return 0; } };


// JUST_RUN_TEST(finae, test)
TEST(finae, test)
{
    std::cout << std::boolalpha;
    std::cout << has_serialize<A>::value << std::endl;
    std::cout << has_serialize<B>::value << std::endl;
    std::cout << has_serialize<C>::value << std::endl;
    std::cout << has_serialize<D>::value << std::endl;
}