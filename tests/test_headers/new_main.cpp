
// Sample custom iterator.
// By perfectly.insane (http://www.dreamincode.net/forums/index.php?showuser=76558)
// From: http://www.dreamincode.net/forums/index.php?showtopic=58468

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <marvin/external_src/json.hpp>
#include <marvin/http/marvin_okv.hpp>
#include <marvin/http/http_header.hpp>

using namespace nlohmann;

TEST_CASE("OKV","")
{
    Marvin::Http::Headers hh;
    Marvin::Http::Headers headers{{
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    OrderedKeyValues hdrs{};
    OrderedKeyValues hdrs02{{
        {"aaa", "AAAAA"},
        {"bb","BBBBB"}
    }};
    OrderedKeyValues hdrs03({
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"}
    });
    OrderedKeyValues hdrs04({
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    });
    std::map<std::string, std::string> vals{
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    };
//    Marvin::Http::Headers h(vals);
//    OrderedKeyValues hdrs06(vals);
    hdrs["one"] = "headers1";
    hdrs["two"] = "headers2";
    hdrs["three"] = "headers3";
    hdrs.set("four", "headers4");

    SECTION("constructors and [] access")
    {
        CHECK(hdrs.size() == 4);
        CHECK(hdrs03.size() == 3);
        CHECK(hdrs02.size() == 2);
        CHECK(hdrs["one"] == "headers1");
        CHECK(hdrs["two"] == "headers2");
        CHECK(hdrs["three"] == "headers3");
        CHECK(hdrs["four"] == "headers4");

        CHECK(hdrs03["aaa"] == "AAAAA");
        CHECK(hdrs03["bb"] == "BBBBB");
        CHECK(hdrs03["ccc"] == "CCCCCC");
    }
    SECTION("set and get")
    {
        CHECK(hdrs.size() == 4);
        hdrs.set("ten", "anotherten");
        CHECK(hdrs.size() == 5);
        CHECK(hdrs.get("ten") == "anotherten");
        CHECK(hdrs["ten"] == "anotherten");
        CHECK(hdrs.size() == 5);
    }
    SECTION("remove")
    {
        CHECK(hdrs.size() == 4);
        hdrs.set("ten", "anotherten");
        CHECK(hdrs.size() == 5);
        hdrs.remove("ten");
        CHECK(hdrs.size() == 4);
        auto f = hdrs.find("ten");
        auto e = hdrs.end();
        auto b = (f == e);
        CHECK( (hdrs.find("ten") == hdrs.end()) );
        CHECK(hdrs.size() == 4);
    }
    SECTION("remove group")
    {
        CHECK(hdrs.size() == 4);
        hdrs.set("ten", "anotherten");
        CHECK(hdrs.size() == 5);
        hdrs.remove({"two","four","ten"});
        CHECK(hdrs.size() == 2);
        CHECK(hdrs["one"] == "headers1");
        CHECK(hdrs["three"] == "headers3");
    }
    SECTION("pointer")
    {
        auto h = hdrs.find("two");
        auto hs = *h;
        auto hpf = h->first;
        auto hps = h->second;
        auto hpk = h->key();
        auto hpv = h->value();
        CHECK(true);
    }
    SECTION("demonstrate_json")
    {
        auto jz = hdrs.jsonizable();
        nlohmann::json j;
        typedef std::vector<std::pair<std::string, std::string>> kvt;
        for(kvt::iterator it = jz.begin(); it != jz.end(); it++) {
            std::string f = (*it).first;
            std::string s = (*it).second;
            nlohmann::json jtmp({ {"key",f}, {"value", s}});
            std::cout << "" << jtmp.dump() << std::endl;
            j.push_back(jtmp);
        }
        std::cout << "" << j.dump() << std::endl;
        std::string js = j.dump();
        nlohmann::json jout = nlohmann::json::parse(js);
        OrderedKeyValues kvOut;
        for (nlohmann::json::iterator it = jout.begin(); it != jout.end(); ++it) {
          auto k = (*it)["key"].get<std::string>();
          auto v = (*it)["value"].get<std::string>();
          std::cout << "k: " << k << " v:" << v << std::endl;
          kvOut[k] = v;
        }
        auto b = (kvOut == hdrs);
        std::cout << "done" << std::endl;
    }
    SECTION("json_with_fucntions")
    {
        nlohmann::json j;
        OrderedKeyValues::to_json(j, hdrs);
        std::cout << j.dump() << std::endl;
        OrderedKeyValues kvout;
        OrderedKeyValues::from_json(j,kvout);
        auto b = (hdrs == kvout);
        std::cout << "done" << std::endl;
    }
    SECTION("json the correct wayt")
    {
        nlohmann::json j(hdrs);
        std::cout << j.dump() << std::endl;
        nlohmann::json jout = nlohmann::json::parse(j.dump());
        OrderedKeyValues kvout = jout.get<OrderedKeyValues>();
        auto b = (hdrs == kvout);
        std::cout << "done" << std::endl;
    }
    SECTION("json headers")
    {
        nlohmann::json j(headers);
        std::cout << j.dump() << std::endl;
        Marvin::Http::Headers h2 = j.get<Marvin::Http::Headers>();
        std::cout << j.dump() << std::endl;

    }
}

#pragma mark - main
#if 0
int main()
{
    OrderedKeyValues hdrs{};
    OrderedKeyValues hdrs02{{
        {"aaa", "AAAAA"},
        {"bb","BBBBB"}
    }};
    OrderedKeyValues hdrs03({
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"}
    });
    hdrs03.remove("bb");
    hdrs["one"] = "header1";
    hdrs["two"] = "headers2";
    hdrs["three"] = "headers3";
    hdrs.set("four", "headers4");
    auto hd2 = hdrs;
    auto bb = (hd2 == hdrs);
    auto i = hdrs.begin();
    auto end = hdrs.end();
    auto f = hdrs.find("two");
    auto b = (f == hdrs.end());
    auto hs = hdrs.has("two");
    for(OrderedKeyValues::Iterator i = hdrs.begin(); i != hdrs.end(); i++)
    {
        auto xx = *i;
        std::cout << "key: " << (*i).key() << " value: " << (*i).value() << " " << hdrs[(*i).key()] << std::endl;
    }
    for(auto& h : hdrs){
        std::cout << h.key() << " " << h.value() << std::endl;
    }

    std::cout << std::endl;

//    std::vector<double> vec;
//    std::copy(point3d.begin(), point3d.end(), std::back_inserter(vec));
//
//    for(std::vector<double>::iterator i = vec.begin(); i != vec.end(); i++)
//    {
//        std::cout << *i << " ";
//    }
//
//    std::cout << std::endl;
    return 0;
}
#endif
