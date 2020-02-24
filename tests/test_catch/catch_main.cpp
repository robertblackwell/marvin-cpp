//#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }
class Fix {
    public:
    Fix(){
        m_index = 999;
        std::cout << __FUNCTION__ << std::endl;
    }
    ~Fix(){
        std::cout << __FUNCTION__ << std::endl;
    }
    protected:
        int getIndex(){ return m_index; }
        int m_index;
};
TEST_CASE_METHOD(Fix, "testname", "[tag]")
{
    auto n = getIndex();
    std::cout << __FUNCTION__ << std::endl;
}
TEST_CASE("testing the factorial function") {
    CHECK(factorial(1) == 1);
  
    CHECK(factorial(2) == 2);
    CHECK(factorial(3) == 16);
    CHECK(factorial(10) == 3628800);
}
