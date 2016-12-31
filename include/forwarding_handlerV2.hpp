//
//  forwarding_handlerV2.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/25/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef forwarding_handlerV2_hpp
#define forwarding_handlerV2_hpp
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <regex>
#include "request.hpp"
#include "request_handler_base.hpp"
#include "rb_logger.hpp"
#include "UriParser.hpp"
#include "request.hpp"
#include "http_connection.hpp"
#include "http_header.hpp"
#include "tunnel_handler.hpp"

enum class ConnectAction;

/**
*  @brief This class implements the proxy forwarding process for http/https protocols.
*  Its a revised version to make way for future requirements of "replaying" an exchange
*   The V2 version of this class makes the composition of functions during the forwarding
*   process more evident.
*  @discussion Reads a message from the downstream client, converts that to a non-proxy request
*  to the targets server, sends that request, collects the response and finally sends that response
*  to the originating client.
*  Along the way it captures (via template parameter TCapture) a summary of the original request and
*  upstream server response and distributes that according to the rules of the particular TCapture object
*/
template<class TCollector> class ForwardingHandlerV2 : public RequestHandlerBase
{
    public:
        ForwardingHandlerV2(boost::asio::io_service& io);
        ~ForwardingHandlerV2();
    
        void handleConnect(
            MessageReaderSPtr           req,
            ConnectionInterfaceSPtr     connPtr,
            HandlerDoneCallbackType     done);

        void handleRequest(
            MessageReaderSPtr           req,
            MessageWriterSPtr           rep,
            HandlerDoneCallbackType done);
    
    private:
        // methods that are used in handleRequest
        void handleRequest_Upstream(
            MessageReaderSPtr req,
            std::function<void(Marvin::ErrorType& err)> upstreamCb
        );
        void handleUpstreamResponseReceived(Marvin::ErrorType& err);
        void makeDownstreamResponse();
        void makeDownstreamErrorResponse(Marvin::ErrorType& err);
        void handleUpgrade();
        void onComplete(Marvin::ErrorType& err);
    
        // methods that are used in handleConnect
        ConnectAction determineConnecAction(std::string host, int port);
        void initiateTunnel();
    
        // utility methods
        void response403Forbidden(MBuffer& sbuf);
        void response200OKConnected(MBuffer& sbuf);
        void response502Badgateway(MBuffer& sbuf);


        /// @brief Only used by the handleConnect method
        ConnectionInterfaceSPtr     _conn;
        MessageReaderSPtr           _req;
        MessageWriterSPtr           _resp;
        RequestUPtr                 _upStreamRequestUPtr;
        HandlerDoneCallbackType     _doneCallback;
        /// this will collect summaries of the req and resp
        std::string                 _scheme;
        std::string                 _host;
        int                         _port;
        TCollector*                 _collector;
    
        /// used for handleConnect - tunnel
        MBufferUPtr                 _initialResponseBuf;
        TunnelHandlerSPtr           _tunnelHandler;
        ConnectionInterfaceSPtr     _downStreamConnection; // used only for tunnel
        HttpConnectionSPtr          _upstreamConnection; // used only for tunnels
    
        /// regexs to define hosts that require mitm not tunnel
        std::vector<std::regex>     _httpsHosts;
    
        /// list of port numbers that can be https mitm'd rather than tunneled
        std::vector<int>            _httpsPorts;

};

#include "forwarding_handlerV2.ipp"

#endif /* forwarding_handlerV2_hpp */
