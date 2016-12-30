//
//  forwarding_handler.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/25/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef forwarding_handler_hpp
#define forwarding_handler_hpp
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "request.hpp"
#include "request_handler_base.hpp"
#include "rb_logger.hpp"
#include "UriParser.hpp"
#include "request.hpp"
#include "http_header.hpp"

/**
**  @brief This class implements the proxy forwarding process for http/https protocols.
**  @discussion Reads a message from the downstream client, converts that to a non-proxy request
**  to the targets server, sends that request, collects the response and finally sends that response
**  to the originating client.
**  Along the way it captures (via template parameter TCapture) a summary of the original request and 
**  upstream server response and distributes that according to the rules of the particular TCapture object
*/
template<class TCollector> class ForwardingHandler : public RequestHandlerBase
{
    public:
        ForwardingHandler(boost::asio::io_service& io);
        ~ForwardingHandler();
    
        void handleConnect(
            MessageReaderSPtr           req,
            ConnectionInterfaceSPtr     connPtr,
            ConnectHandlerHijackCallbackType   hijackConnection);

        void handleRequest(
            MessageReaderSPtr           req,
            MessageWriterSPtr           rep,
            HandlerDoneCallbackType done);
    
    private:
        void handleUpstreamResponseReceived(Marvin::ErrorType& err);
        void makeDownstreamResponse();
        void makeDownstreamErrorResponse(Marvin::ErrorType& err);
        void handleUpgrade();
        void onComplete(Marvin::ErrorType& err);

        /// @brief Only used by the handleConnect method
        ConnectionInterfaceSPtr     _conn;
        MessageReaderSPtr           _req;
        MessageWriterSPtr           _resp;
        RequestUPtr                 _upStreamRequestUPtr;
        HandlerDoneCallbackType     _doneCallback;
        /// this will collect summaries of the req and resp
        std::string                 _scheme;
        std::string                 _host;
        TCollector*                 _collector;
};

#include "forwarding_handler.ipp"

#endif /* forwarding_handler_hpp */
