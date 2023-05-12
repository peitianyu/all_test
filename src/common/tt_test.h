#ifndef __COMMON_TEST_H__
#define __COMMON_TEST_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/time.h>

class Tester
{
public:
	Tester(const std::string &name, uint line);
	~Tester();
	Tester &Is(bool x, const std::string &msg = "");
private:
	std::string m_name;
	uint m_line;
	bool m_ok;
};

#define ASSERT_TRUE(x) Tester(__FILE__, __LINE__).Is((x), #x)
#define ASSERT_FALSE(x) Tester(__FILE__, __LINE__).Is(!(x), #x)
#define ASSERT_EQ(a, b) Tester(__FILE__, __LINE__).Is((a) == (b), #a " == " #b)
#define ASSERT_NE(a, b) Tester(__FILE__, __LINE__).Is((a) != (b), #a " != " #b)
#define ASSERT_LT(a, b) Tester(__FILE__, __LINE__).Is((a) < (b), #a " < " #b)
#define ASSERT_LE(a, b) Tester(__FILE__, __LINE__).Is((a) <= (b), #a " <= " #b)
#define ASSERT_GT(a, b) Tester(__FILE__, __LINE__).Is((a) > (b), #a " > " #b)
#define ASSERT_GE(a, b) Tester(__FILE__, __LINE__).Is((a) >= (b), #a " >= " #b)

class RegisterTest
{
public:
	static RegisterTest *GetInstance(){
		static RegisterTest instance;
		return &instance;
	}

	void Register(const std::string &name, void (*test)());
	bool RunAllTests();
	std::string *m_run_name = nullptr;
	std::vector<std::string> *m_dont_run_names = nullptr;
private:
	RegisterTest();
	~RegisterTest();
	struct ContextTest{
		std::string name;
		void (*test)();
	};
	uint GetTime();
	void RunTest(const ContextTest& test);
	void DontRun();
	bool JustRun();
	std::vector<ContextTest> *m_tests;
};

#ifdef TEST_ENABLE
#define DONT_RUN_TEST(base, name)                                                               \
	struct dont##name##_Test                                                                    \
	{                                                                                           \
		dont##name##_Test()                                                                     \
		{                                                                                       \
			if (!RegisterTest::GetInstance()->m_dont_run_names)                                 \
				RegisterTest::GetInstance()->m_dont_run_names = new std::vector<std::string>(); \
			RegisterTest::GetInstance()->m_dont_run_names->push_back(#base "." #name);          \
		}                                                                                       \
	};                                                                                          \
	dont##name##_Test g_dont##name##_Test;
#define JUST_RUN_TEST(base, name)  															 	\
	struct just##name##_Test																	\
	{ 																							\
		just##name##_Test() 																	\
		{  																						\
			if(!RegisterTest::GetInstance()->m_run_name) 						 				\
				RegisterTest::GetInstance()->m_run_name = new std::string();					\
			RegisterTest::GetInstance()->m_run_name->assign(#base "." #name);					\
		} 																						\
	}; 																							\
	just##name##_Test g_just##name##_Test;
#define TEST(base, name)                              											\
	struct base##name##_Test                          											\
	{                                                 											\
		base##name##_Test(){                          											\
			RegisterTest::GetInstance()->Register(#base "." #name, &base##name##_Test::Run); 	\
		}                                             											\
		static void Run();                            											\
	};                                                											\
	base##name##_Test g_##base##name##_Test;          											\
	void base##name##_Test::Run()

#define RUN_ALL_TESTS() RegisterTest::GetInstance()->RunAllTests()
#else
#define TEST(base, name) void base##name##_Test()
#define RUN_ALL_TESTS() false
#endif // TEST_ENABLE

#endif // __COMMON_TEST_H__