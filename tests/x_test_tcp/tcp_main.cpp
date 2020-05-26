#include <cstdlib>
#include <iostream>
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h> 
    
TEST_CASE("subcase test")
{
    std::string s = std::getenv("TEST_SERVER_BASEURL");
    std::cout << "tcp Testcase TEST_SERVER_BASEURL: " << s << std::endl;
}
