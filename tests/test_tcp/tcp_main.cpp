#include <cstdlib>
#include <iostream>
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h> 
// #include <marvin/external_src/rb_logger/rb_logger.hpp>
// RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
    
TEST_CASE("subcase test")
{
    std::string s = std::getenv("TEST_SERVER_BASEURL");
    std::cout << "tcp Testcase TEST_SERVER_BASEURL: " << s << std::endl;
}
