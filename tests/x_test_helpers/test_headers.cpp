//
//  test_json.cpp
//  test_proxy
//
//  Created by ROBERT BLACKWELL on 1/6/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//
#include <doctest/doctest.h>
#include <marvin/http/headers_v2.hpp>
#include "test_headers.hpp"

struct kv_t {
    std::string k;
    std::string v;
};

std::vector<std::pair<std::string, std::string>> jsonParseHeaders(nlohmann::json jsonHeaders) {
    typedef std::vector<kv_t> header_list;
    header_list hl{};
    std::vector<std::pair<std::string, std::string>> hm{};
    for (nlohmann::json::iterator it = jsonHeaders.begin(); it != jsonHeaders.end(); ++it) {
//        std::cout << it.key() << " : " << it.value() << "\n";
        auto z = *it;
        std::string k = it.key();
        if (it.value().is_string()) {
            std::string v = it.value();
            std::pair<std::string, std::string> akv(k, v);
            hm.push_back(akv);
        } else if (it.value().is_number_integer()) {
            int v = it.value().get<int>();
            std::string vstr = std::to_string(v);
            std::pair<std::string, std::string> akv(k, vstr);
            hm.push_back(akv);
        } else if (it.value().is_number_float()) {
            double v = it.value().get<float>();
            std::string vstr = std::to_string(v);
            std::pair<std::string, std::string> akv(k, vstr);
            hm.push_back(akv);
        } else {
            throw "invalid type of value in header list";
        }
    }
    return hm;
}



namespace test{
namespace helpers{
Marvin::Http::HeadersV2 headersFromJson(nlohmann::json& j)
{
    auto h = jsonParseHeaders(j);
    Marvin::Http::HeadersV2 result(h);
    return result;
}
bool checkHeaders(Marvin::Http::HeadersV2& h1, Marvin::Http::HeadersV2 h2)
{
    bool result = (h1.size() == h2.size());
    CHECK(h1.size() == h2.size());
    Marvin::Http::HeadersV2 tmp_h, other_h;
    if (h1.size() > h2.size()) {
        tmp_h = h1; other_h = h2;
    } else {
        tmp_h = h2; other_h = h1;
    };
    for(auto const& it : tmp_h) {
        std::string k = it.first;
        auto x2 = other_h.find(k);
        bool d = (x2 != other_h.end());
        result = result && d;
        INFO("k " + k + " not in other_h");
        CHECK(d);
        auto x = h2.atKey(k).get();
//        std::cout << k << std::endl;
    }
    return result;
}

}
}

