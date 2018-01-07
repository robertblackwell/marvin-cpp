//
//  test_json.hpp
//  test_proxy
//
//  Created by ROBERT BLACKWELL on 1/6/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//

#ifndef test_json_hpp
#define test_json_hpp
#include <string>
#include <map>
#include "http_header.hpp"
#include "json.hpp"
namespace test {
namespace helpers {
    HttpHeadersType  headersFromJson(nlohmann::json& j);
    bool checkHeaders(HttpHeadersType& h1, HttpHeadersType h2);
}
}

#endif /* test_json_hpp */
