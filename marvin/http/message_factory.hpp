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
    void make_request(MessageBase& msg, HttpMethod method, Marvin::Uri& uri);
    void make_proxy_request(MessageBase& msg, HttpMethod method, Marvin::Uri& uri);
    void make_proxy_connect_request(MessageBase& msg, std::string server, std::string port);

    void make_response_403_forbidden(MessageBase& msg);
    void make_response_200_OK_connected(MessageBase& msg);
    void make_response_502_badgateway(MessageBase& msg);
    
} // namespace Marvin
#endif /* http_request_hpp */
