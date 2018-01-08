
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
#include <catch/catch.hpp>
#include "marvin_okv.hpp"

TEST_CASE("OKV","")
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
    OrderedKeyValues hdrs04({
        {"aaa", "AAAAA"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    });
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
    SECTION("copyExcept")
    {
    
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
