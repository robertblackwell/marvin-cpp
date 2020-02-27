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
#include <marvin/http/http_header.hpp>
#include <marvin/external_src/json.hpp>
namespace test {
namespace helpers {
    Marvin::Http::Headers  headersFromJson(nlohmann::json& j);
    bool checkHeaders(Marvin::Http::Headers& h1, Marvin::Http::Headers h2);
}
}

#endif /* test_json_hpp */
