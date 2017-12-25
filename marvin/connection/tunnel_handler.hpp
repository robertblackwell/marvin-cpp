//
//  TunnelHandler.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef tunnel_handler_hpp
#define tunnel_handler_hpp

#include <stdio.h>
#include <memory>
#include "marvin_error.hpp"
#include "tcp_connection.hpp"
#include "half_tunnel.hpp"

class TunnelHandler;
typedef std::shared_ptr<TunnelHandler> TunnelHandlerSPtr;
typedef std::unique_ptr<TunnelHandler> TunnelHandlerUPtr;

class TunnelHandler
{
    public:
        TunnelHandler(
            ISocketSPtr  downStreamConnection,
            TCPConnectionSPtr      upstreamConnection
        );
        ~TunnelHandler();
        void start(std::function<void(Marvin::ErrorType& err)> cb);

    private:
        void tryDone();
        std::function<void(Marvin::ErrorType& err)> _callback;
        ISocketSPtr     _downstreamConnection;
//        TCPConnectionSPtr          _upstreamConnection;
        ISocketSPtr     _upstreamConnection;
    
        HalfTunnelUPtr              _upstreamHalfTunnel;
        HalfTunnelUPtr              _downstreamHalfTunnel;
    
        bool                        _upstreamDone;
        Marvin::ErrorType           _upstreamErr;
        bool                        _downstreamDone;
        Marvin::ErrorType           _downstreamErr;
        Marvin::ErrorType           _firstErr;
};

#endif /* tunnel_handler_hpp */
