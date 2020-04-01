
// Sample custom iterator.
// By perfectly.insane (http://www.dreamincode.net/forums/index.php?showuser=76558)
// From: http://www.dreamincode.net/forums/index.php?showtopic=58468

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <regex>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <json/json.hpp>
#include <marvin/http/ordered_key_value.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/headers_v2.hpp>

using namespace Marvin;
using namespace Http;

///
/// KeepAlive is true if:
///     there is a connection header that contains the string "[ ,]keep-alive[ ,]" case independent
///     or
///     there is NOT a connection header that contain the string 'close' case insensitive
///         and the msg http version is 1.1
///
/// Keepalive is explicitly false
///     there is a connection header that contain the string 'close' case insensitive
///     or
///     there is NOT a connection header that contain the string 'keep-alive' case insensitive
///         and the msg http version is 1.0
///
#if 0
bool isKeepAlive(Marvin::Http::MessageBase& msg)
{
    if (msg.hasHeader(Headers::Name::Connection)) {
        return isKeepAlive(msg.getHeader(Headers::Name::Connection));
    } else {
        if (msg.httpVersMinor() == 1) {
            return true;
        } else {
            return false;
        }
    }
    Headers hdrs = msg.getHeader(Headers::Name::Connection);
    return isKeepAlive(hdrs);
}
bool isKeepAlive(MessageBaseSPtr msg_sptr)
{
    return isKeepAlive(*msg_sptr);
}

bool isKeepAlive(Marvin::Http::Headers& headers)
{
    OrderedKeyValues::Iterator it = headers.find("Connection");
    if (it  == headers.end()) {
        return false;
    }
    std::string value = (*it).value();
    if (isKeepAlive(value)) {
        return true;
    }
    return false;
}
#endif
bool isKeepAlive(std::string value)
{
    std::regex r("keep-alive", std::regex::icase);
    auto x = std::regex_search(value, r);
    return x;
}

TEST_CASE("new headres")
{
    Marvin::Http::HeadersV2 headers;
    CHECK(headers.size() == 0);
    headers.setAtKey("Connection", "keep-alive");
    CHECK(headers.size() == 1);
    CHECK( ( !!headers.atKey(Marvin::Http::HeadersV2::Connection )) );
    CHECK( ( !!headers.atKey("Connection")) );
    CHECK( ( !!headers.atKey("conNecTion")) );
    CHECK( ( !!headers.atKey("CONNECTION")) );
    CHECK(headers.atKey("Connection").get() == "keep-alive");
    CHECK(headers.findAtIndex("Connection").get() == 0);
    auto it1 = headers.find("connection");
    auto it2 = headers.end();
    CHECK( !(headers.find("connection") == headers.end()) );

    headers.setAtKey("Content-Length", "33");
    CHECK(headers.size() == 2);
    CHECK( ( !!headers.atKey(Marvin::Http::HeadersV2::ContentLength )) );
    CHECK( ( !!headers.atKey("Content-length")) );
    CHECK(headers.findAtIndex("content-length").get() == 1);
    CHECK( !!(headers.find("content-length") != headers.end()) );

    CHECK( (! headers.atKey("some-other-key")) );
    CHECK( (! headers.findAtIndex("someother-keyvalue")) );
    CHECK( (headers.find("someother-key") == headers.end()) );
    
    headers.removeAtKey("connection");
    CHECK(headers.size() == 1);
    CHECK( ( !headers.atKey("CONNECTION")) );
    CHECK( ( !headers.findAtIndex("CONNECTION")) );
    CHECK( (headers.find("connection") == headers.end()) );
    CHECK( ( !!headers.atKey("COntent-length")) );
    CHECK( ( !!headers.findAtIndex("COntent-length")) );
    CHECK( !!(headers.find("content-length") != headers.end()) );

    headers.removeAtKey("content-length");
    CHECK(headers.size() == 0);
    CHECK( ( !headers.atKey("COntent-length")) );
    CHECK( ( !headers.findAtIndex("COntent-length")) );
    CHECK( !!(headers.find("content-length") == headers.end()) );

}

using namespace nlohmann;
TEST_CASE("header alternative")
{
    std::pair<std::string, std::string> a{"one", "two"};
    std::pair<std::string, std::string> b{"one", "two1"};
    std::pair<std::string, std::string> c{"one", "two"};
    
    bool xb = (a == b);
    bool xb2 = (a == c);

    std::vector<std::pair<std::string, std::string>> hdr{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    auto x = hdr[0];

}
TEST_CASE("headers")
{
    Marvin::Http::Headers hh;
    Marvin::Http::Headers headers{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    CHECK(isKeepAlive(std::string(" this has keep-alive embedded")));
    CHECK(isKeepAlive(std::string(" this has Keep-alive embedded")));
    CHECK(isKeepAlive(std::string(" this, has, Keep-Alive, embedded")));
    CHECK(! isKeepAlive(std::string(" this, has, Close, embedded")));
    auto value = headers["Connection"];
    CHECK(value == "Keep-Alive, another1, another2");
    auto b = isKeepAlive(value);
    // CHECK(b);

    
}
TEST_CASE("OKV")
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

    SUBCASE("constructors and [] access")
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
    SUBCASE("set and get")
    {
        CHECK(hdrs.size() == 4);
        hdrs.set("ten", "anotherten");
        CHECK(hdrs.size() == 5);
        CHECK(hdrs.get("ten") == "anotherten");
        CHECK(hdrs["ten"] == "anotherten");
        CHECK(hdrs.size() == 5);
    }
    SUBCASE("remove")
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
    SUBCASE("remove group")
    {
        CHECK(hdrs.size() == 4);
        hdrs.set("ten", "anotherten");
        CHECK(hdrs.size() == 5);
        hdrs.remove({"two","four","ten"});
        CHECK(hdrs.size() == 2);
        CHECK(hdrs["one"] == "headers1");
        CHECK(hdrs["three"] == "headers3");
    }
    SUBCASE("pointer")
    {
        auto h = hdrs.find("two");
        auto hs = *h;
        auto hpf = h->first;
        auto hps = h->second;
        auto hpk = h->key();
        auto hpv = h->value();
        CHECK(true);
    }
    SUBCASE("demonstrate_json")
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
    SUBCASE("json_with_fucntions")
    {
        nlohmann::json j;
        OrderedKeyValues::to_json(j, hdrs);
        std::cout << j.dump() << std::endl;
        OrderedKeyValues kvout;
        OrderedKeyValues::from_json(j,kvout);
        auto b = (hdrs == kvout);
        std::cout << "done" << std::endl;
    }
    SUBCASE("json the correct wayt")
    {
        nlohmann::json j(hdrs);
        std::cout << j.dump() << std::endl;
        nlohmann::json jout = nlohmann::json::parse(j.dump());
        OrderedKeyValues kvout = jout.get<OrderedKeyValues>();
        auto b = (hdrs == kvout);
        std::cout << "done" << std::endl;
    }
    SUBCASE("json headers")
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
