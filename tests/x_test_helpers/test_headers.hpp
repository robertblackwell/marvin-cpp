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
#include <marvin/http/header_fields.hpp>
#include <json/json.hpp>
namespace test {
namespace helpers {
    Marvin::Http::HeadersV2 headersFromJson(nlohmann::json& j);
}
}

#endif /* test_json_hpp */
