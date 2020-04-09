#include <cstdlib>
#include <iostream>
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h> 
// #include <marvin/external_src/trog/trog.hpp>
// Trog_SETLEVEL(LOG_LEVEL_WARN)
    
TEST_CASE("subcase test")
{
    std::string s = std::getenv("TEST_SERVER_BASEURL");
    std::cout << "tcp Testcase TEST_SERVER_BASEURL: " << s << std::endl;
}
