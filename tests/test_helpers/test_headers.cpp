//
//  test_json.cpp
//  test_proxy
//
//  Created by ROBERT BLACKWELL on 1/6/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//
#include <catch/catch.hpp>
#include "http_header.hpp"
#include "test_headers.hpp"
namespace test{
namespace helpers{
HttpHeadersType headersFromJson(nlohmann::json& j)
{
    std::map<std::string, std::string> result;
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
        std::cout << it.key() << " : " << it.value() << "\n";
        result[it.key()] = it.value();
    }
    return result;
}
bool checkHeaders(HttpHeadersType& h1, HttpHeadersType h2)
{
    bool result = (h1.size() == h2.size());
    CHECK(h1.size() == h2.size());
    HttpHeadersType tmp_h, other_h;
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
        auto x = h2[k];
//        std::cout << k << std::endl;
    }
    return result;
}

}
}

