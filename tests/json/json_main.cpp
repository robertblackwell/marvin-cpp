#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "json.hpp"
#include "rb_logger.hpp"
#define RBLOGGER_ON

RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

using json = nlohmann::json;

int main()
{
    json j1 = {
        {"one", "11111"},
        {"two", "22222"}
    };
    nlohmann::json j2 = json::object({{"aaa", "AAAA"}, {"BBBB","bbbb"}});
    auto j3 = json::parse("{ \"happy\": true, \"pi\": 3.141 }");
    auto x1 = j2["aaa"];
    auto x2 = j2["pi"];
    
    std::map<std::string, std::string> c_map { {"one", "111"}, {"two", "22323"}, {"three", "1243453"} };
    json j_map(c_map);
    auto s_map = j_map.dump();
    json j_map2 = json::parse(s_map);
    std::string ss = j_map2["one"];
    json j_details;
    j_details["body"] = "this is a body string";
    j_details["headers"] = c_map;
    
    std::string sds = j_details.dump();
    
    return 0;
}
