//
//  http_request.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 1/7/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//

#ifndef marvin_http_hpp
#define marvin_http_hpp

#include <cstdio>
#include <marvin/http/message_base.hpp>
#include <marvin/http/http_method.hpp>
#include <marvin/http/uri.hpp>
namespace Marvin {
	/**
	* \ingroup http
	* \brief construct http messages of various types.
	*/
    void makeRequest(MessageBase& msg, HttpMethod method, Marvin::Uri& uri);
    void makeProxyRequest(MessageBase& msg, HttpMethod method,  Marvin::Uri& uri);
    void makeProxyConnectRequest(MessageBase& msg, std::string server, std::string port);

    void makeResponse403Forbidden(MessageBase& msg);
    void makeResponse200OKConnected(MessageBase& msg);
    void makeResponse502Badgateway(MessageBase& msg);
    
} // namespace Marvin
#endif /* http_request_hpp */
