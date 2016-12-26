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
#include "request.hpp"
#include "request_handler_base.hpp"
/**
* @brief This class implements the proxy forwarding process for http/https protocols.
*/
class ForwardingHandler : public RequestHandlerBase
{
    public:
        ForwardingHandler(boost::asio::io_service& io);
        ~ForwardingHandler();
    
        void handleConnect(
            MessageReaderSPtr           req,
            ConnectionInterfaceSPtr     connPtr,
            HandlerDoneCallbackType done);

        void handleRequest(
            MessageReaderSPtr req,
            MessageWriterSPtr rep,
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
};

#endif /* forwarding_handler_hpp */
