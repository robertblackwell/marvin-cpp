//
//  forward_helpers.hpp
//  marvin
//
//  Created by ROBERT BLACKWELL on 12/29/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef marvin_forward_helpers_hpp
#define marvin_forward_helpers_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <regex>
#include "request.hpp"
#include "request_handler_base.hpp"
#include "rb_logger.hpp"
#include "UriParser.hpp"
#include "client.hpp"
#include "message.hpp"
#include "tcp_connection.hpp"
#include "http_header.hpp"
#include "tunnel_handler.hpp"
namespace helpers {

std::string base64Encode(std::string& source);

http::url decodeUri(MessageReaderSPtr requestSPtr);
void applyUri(MessageBaseSPtr msg, std::string uri);
void fillRequestFromUri(MessageBase& msg, std::string uri_in, bool absolute = false);

void removeHopByHop(MessageBaseSPtr msgSPtr, std::string connectionValue);

/// \brief apply proxy rules, make an upstream request from the original client request
void makeUpstreamRequest(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr);

/// \brief makes a response to send down to client from a response from the up stream server
/// and the error_code returned from the round trip to upstream server.
void makeDownstreamResponse(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err);

/// \brief makes a down stream response if round trip returned an error_code
void makeDownstreamErrorResponse(MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err);

/// \brief makes a downstream response from a response received from upstream server.
void makeDownstreamGoodResponse(MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr);


bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response);
bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response);
}
#endif /* forward_helpers_hpp */
