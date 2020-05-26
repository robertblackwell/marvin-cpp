#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <json/json.hpp>

using json = nlohmann::json;
std::map<std::string, std::string> parseJson(json j)
{
//    auto t = j["one"];
    std::map<std::string, std::string> result;
    for(auto const& k : j) {
        auto t = k.size();
        auto one = k[0];
        auto two = k[1];
        result[one] = two;
        std::cout << "k : " << k[0] << " v: "<< k[1] << std::endl;
    }
    return result;
}
std::map<std::string, std::string> parseJson2(json j)
{
    std::map<std::string, std::string> result;
    for (json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
        result[it.key()] = it.value();
    }
    return result;
}


int main()
{
    json j1 = {
        {"one", "11111"},
        {"two", "22222"}
    };
    json j1_array = json::array({
        {"one", "11111"},
        {"two", "22222"}
    });
    auto ss1 = j1.dump();
    auto ss2 = j1_array.dump();
    auto xx = parseJson(j1_array);
    auto xx2 = parseJson2(j1);
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
//    std::map<std::string, std::string> m2 = j_map2.get<decltype(m2)>();
    std::string sds = j_details.dump();
    
    return 0;
}
