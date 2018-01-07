//
//  http_request.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 1/7/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//

#ifndef marvin_http_hpp
#define marvin_http_hpp

#include <stdio.h>
#include "message.hpp"
#include "mavin_uri.hpp"
namespace Marvin {
namespace Http {
    void makeRequest(MessageBase& msg, Marvin::Uri& uri);
    void makeProxyRequest(MessageBase& msg, Marvin::Uri& uri);
    
    void makeResponse403Forbidden(MessageBase& msg);
    void makeResponse200OKConnected(MessageBase& msg);
    void makeResponse502Badgateway(MessageBase& msg);
    
} // namespace Http
} // namespace Marvin
#endif /* http_request_hpp */
