#ifndef marvin_tester_hpp
#define marvin_tester_hpp
void test_true(std:string f, std::string line, std::string func, bool);
void test_false(std:string f, std::string line, std::string func, bool);
void xyz(z) {printf("", z);}
//#define EXPECTTRUE((test))  xyz(test);
/*
#define xEXPECTTRUE((test)) test_true(__file__, __line__, __function__, (test));

#define EXPECTTRUE((test)) test_false(__file__, __line__, __function__, (test));

#define TEST(suite, testname) \
void impl_test_suite_name(); \
void test_suite_testname() { \
    impl_test_suite_name(); \
} \
void impl_test_suite_name() 
^/
#endif
