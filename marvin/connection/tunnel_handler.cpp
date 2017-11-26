//
//  TunnelHandler.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <memory>
#include "marvin_error.hpp"
#include "tunnel_handler.hpp"
#include "half_tunnel.hpp"

TunnelHandler::TunnelHandler(
    ConnectionInterfaceSPtr     downstreamConnection,
    TCPConnectionSPtr          upstreamConnection
)
{
    _downstreamConnection   = downstreamConnection;
    _upstreamConnection     = (ConnectionInterfaceSPtr)upstreamConnection;
    
    _upstreamHalfTunnel     =  std::unique_ptr<HalfTunnel>( new HalfTunnel(_downstreamConnection, _upstreamConnection));
    _downstreamHalfTunnel   =  std::unique_ptr<HalfTunnel>(new HalfTunnel(_upstreamConnection, _downstreamConnection ));

    _upstreamDone = false;
    _downstreamDone = false;
    _firstErr = Marvin::make_error_ok();
    
}
TunnelHandler::~TunnelHandler(){};

void TunnelHandler::start(std::function<void(Marvin::ErrorType& err)> cb)
{
    _callback = cb;
    
    /// start both halves, downstream first as there is not likely to be traffic that way until the upstream starts
    /// we are done when they are both done
    /// the error to record is the one that strikes first.
    /// we done need to force close anything as the servers ro client should eventually close their end and we will ehar about it
    _downstreamHalfTunnel->start([this](Marvin::ErrorType& err){
        _downstreamDone = true;
        _downstreamErr = err;
        if( (_firstErr == Marvin::make_error_ok()) && (err != Marvin::make_error_ok() ) )
            _firstErr = err;
        tryDone();
    });
    _upstreamHalfTunnel->start([this](Marvin::ErrorType& err){
        _upstreamDone = true;
        _upstreamErr = err;
        if( (_firstErr == Marvin::make_error_ok()) && (err != Marvin::make_error_ok() ) )
            _firstErr = err;
        tryDone();
    });
}
void TunnelHandler::tryDone()
{
    if( _upstreamDone && _downstreamDone )
    {
        _callback(_firstErr);
    }
}
