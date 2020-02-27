#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <marvin/external_src/json.hpp>
#include <marvin/external_src/rb_logger.hpp>
//#include "test_macros.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
void test(std::string f, std::string F, int ln, bool x) {
    std::cout << f << ln << x << std::endl;
}
#define TT(a) printf("%d", a);
#define foo(x) test((char*)__FILE__, (char*)__FUNCTION__, __LINE__, (x) );
int main()
{
    foo(false)
}
