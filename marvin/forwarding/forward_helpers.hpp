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
#include <marvin/server/request_handler_base.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/client/client.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/tunnel_handler.hpp>
namespace helpers {

std::string base64Encode(std::string& source);

http::url decodeUri(MessageReaderSPtr requestSPtr);

/// \brief applies the Uri to a MessageBase to make a PROXY request where the uri in
/// the message is an absolute uri; fills in the message uri field and host header
void applyUriProxy(Marvin::Http::MessageBaseSPtr msg, Marvin::Uri& uri);

/// \brief applies the Uri to a MessageBase to make a NON PROXY request where the uri in
/// the message is an relative uri; fills in the message uri field and host header
void applyUriNonProxy(Marvin::Http::MessageBaseSPtr msg, Marvin::Uri& uri);

//void applyUri(MessageBaseSPtr msg, Marvin::Uri& uri, bool proxy);

//void applyUri(MessageBaseSPtr msg, std::string uri);

//void fillRequestFromUri(MessageBase& msg, std::string uri_in, bool absolute = false);

void removeHopByHop(Marvin::Http::MessageBaseSPtr msgSPtr, std::string connectionValue);

/// \brief apply proxy rules, make an upstream request from the original client request
void makeUpstreamRequest(Marvin::Http::MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr);

/// \brief makes a response to send down to client from a response from the up stream server
/// and the error_code returned from the round trip to upstream server.
void makeDownstreamResponse(Marvin::Http::MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err);

/// \brief makes a down stream response if round trip returned an error_code
void makeDownstreamErrorResponse(Marvin::Http::MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err);

/// \brief makes a downstream response from a response received from upstream server.
void makeDownstreamGoodResponse(Marvin::Http::MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr);


bool apply_connection_close(MessageReaderSPtr req, Marvin::Http::MessageBaseSPtr response);
bool apply_keepalive_rules(MessageReaderSPtr req, Marvin::Http::MessageBaseSPtr response);
}
#endif /* forward_helpers_hpp */
