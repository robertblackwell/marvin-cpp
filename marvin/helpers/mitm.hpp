//
//  forward_helpers.hpp
//  marvin
//
//  Created by ROBERT BLACKWELL on 12/29/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef marvin_helpers_mitm_hpp
#define marvin_helpers_mitm_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <regex>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/http/uri.hpp>

namespace Marvin {
/**
* \ingroup mitmapp
*
*/
namespace Helpers {
std::string base64Encode(std::string& source);

http::url decodeUri(MessageReaderSPtr requestSPtr);

/// \brief Inserts the Uri into the target field and host field of the message in a way
/// consistent with the requirements of a proxy request. The target should be absolute.
/// the host should include the port
void apply_uri_proxy(MessageBaseSPtr msg, Uri& uri);

/// \brief applies the Uri to a MessageBase to make a NON PROXY request where the uri in
/// the message is an relative uri; fills in the message uri field and host header
void apply_uri_non_proxy(MessageBaseSPtr msg, Uri& uri);

void remove_hop_by_hop(MessageBaseSPtr msgSPtr, std::string connectionValue);

/// \brief apply proxy rules, make an upstream request from the original client request
void make_upstream_request(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr);

/// make an upstream request from a client request proxy rules are not applied as
/// the client is assuming we are in tunnel mode
void make_upstream_https_request(MessageBaseSPtr upstreamRequest, MessageReaderSPtr  requestSPtr);

/// \brief makes a response to send down to client from a response from the up stream server
/// and the error_code returned from the round trip to upstream server.
void make_downstream_response(MessageBaseSPtr msg, MessageReaderSPtr resp, ErrorType& err);

/// \brief makes a down stream response if round trip returned an error_code
void make_downstream_error_response(Marvin::MessageBaseSPtr msg, MessageReaderSPtr resp, Marvin::ErrorType& err);

/// \brief makes a downstream response from a response received from upstream server.
void make_downstream_good_response(Marvin::MessageBaseSPtr downstream, MessageReaderSPtr responseSPtr);


bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response);
bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response);
} // namespace
} // namespace
#endif /* forward_helpers_hpp */
