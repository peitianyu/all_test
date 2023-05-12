#include "tt_test.h"


Tester::Tester(const std::string &name, uint line) : m_name(name), m_line(line), m_ok(true) {}
Tester::~Tester(){ if(!m_ok) exit(1); }
Tester &Tester::Is(bool x, const std::string &msg)
{
	if (!x){
		std::cout << "\033[31m" << "[      !OK ] " << m_name << ":" << m_line << " " << msg << "\033[0m" << std::endl;
		m_ok = false;
	}
	return *this;
}

void RegisterTest::Register(const std::string &name, void (*test)()){
	if(!m_tests){ m_tests = new std::vector<ContextTest>();}
	m_tests->push_back(ContextTest{name, test});
}

bool RegisterTest::RunAllTests(){		
	if(!m_tests) return false;

	if(m_run_name) return JustRun();

	DontRun();

	std::cout << "\033[32m" << "[==========] Running " << m_tests->size() << " tests." << "\033[0m" << std::endl;
	for(auto &test : *m_tests) RunTest(test);
	std::cout << "\033[32m" << "[ ALL TESTS PASSED SUCCESS ]" << "\033[0m" << std::endl;
	return true;
}

RegisterTest::RegisterTest() : m_run_name(nullptr), m_dont_run_names(nullptr), m_tests(nullptr) {}
RegisterTest::~RegisterTest(){ if(m_tests) delete m_tests; if(m_dont_run_names) delete m_dont_run_names; if(m_run_name) delete m_run_name; }

uint RegisterTest::GetTime(){
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000 + time.tv_usec / 1000;
}

void RegisterTest::RunTest(const ContextTest& test){
	std::cout << "\033[32m" << "[ RUN      ] " << test.name << "\033[0m" << std::endl;
	uint start_time = GetTime();
	test.test();
	std::cout << "\033[32m" << "[      OK  ] " << test.name  << " (" << (GetTime() - start_time) << " ms)" << "\033[0m" << std::endl;
	std::cout << "\033[32m" << "[-----------------------------------------------------------------------]" << "\033[0m" << std::endl;
}

void RegisterTest::DontRun(){
	std::vector<ContextTest> *tests = new std::vector<ContextTest>();
	for(auto &test : *m_tests){
		bool is_dont_run = false;
		for(auto &name : *m_dont_run_names){
			if(test.name == name){
				is_dont_run = true;
				break;
			}
		}
		if(!is_dont_run) tests->push_back(test);
	}

	delete m_tests;
	m_tests = tests;
}

bool RegisterTest::JustRun()
{
	for(auto &test : *m_tests) if(test.name == *m_run_name){RunTest(test); return true;}
	std::cout << "\033[31m" << "[  ERROR   ] " << "Can't find test: " << *m_run_name << "\033[0m" << std::endl;
	return false;
}
