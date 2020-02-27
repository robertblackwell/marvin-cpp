//
//  TunnelHandler.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include <memory>
#include <marvin/error/marvin_error.hpp>
#include <marvin/connection/tunnel_handler.hpp>
#include <marvin/connection/half_tunnel.hpp>
#include <marvin/external_src/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
std::string traceTunnel(TunnelHandlerUPtr t_ptr)
{
    std::stringstream ss;
    ss << "TNLH[" << std::hex << (long)(void*)t_ptr.get() << std::dec << "]: ";
    return ss.str();
}
std::string traceTunnel(TunnelHandlerSPtr t_ptr)
{
    std::stringstream ss;
    ss << "TNLH[" << std::hex << (long)(void*)t_ptr.get() << std::dec << "]: ";
    return ss.str();
}

TunnelHandler::TunnelHandler(
    boost::asio::io_service& io,
    ISocketSPtr         downstreamConnection,
    TCPConnectionSPtr   upstreamConnection
) : m_io(io)
{
    m_downstream_connection   = downstreamConnection;
    m_upstream_connection     = (ISocketSPtr)upstreamConnection;
    m_first_read_timeout_millisecs = 45000;
    m_subsequent_read_timeout_millisecs = 15000;
    m_upstream_halftunnel     =  std::unique_ptr<HalfTunnel>(
        new HalfTunnel(
            m_downstream_connection, m_upstream_connection, m_first_read_timeout_millisecs, m_subsequent_read_timeout_millisecs
        )
    );
    m_downstream_halftunnel   =  std::unique_ptr<HalfTunnel>(
        new HalfTunnel(
            m_upstream_connection, m_downstream_connection, m_first_read_timeout_millisecs, m_subsequent_read_timeout_millisecs
        )
    );

    m_upstream_done = false;
    m_downstream_done = false;
    m_first_err = Marvin::make_error_ok();
    
}
TunnelHandler::~TunnelHandler(){};

void TunnelHandler::start(std::function<void(Marvin::ErrorType& err)> cb)
{
    m_callback = cb;
//    m_downstream_connection->setReadTimeout(20000);
//    m_upstream_connection->setReadTimeout(20000);
    /// start both halves, downstream first as there is not likely to be traffic that way until the upstream starts
    /// we are done when they are both done
    /// the error to record is the one that strikes first.
    /// we done need to force close anything as the servers or client should eventually close their end and we will hear about it
    m_downstream_halftunnel->start([this](Marvin::ErrorType& err){
        m_downstream_done = true;
        m_downstream_err = err;
        if( (m_first_err == Marvin::make_error_ok()) && (err != Marvin::make_error_ok() ) )
            m_first_err = err;
        tryDone();
    });
    m_upstream_halftunnel->start([this](Marvin::ErrorType& err){
        m_upstream_done = true;
        m_upstream_err = err;
        if( (m_first_err == Marvin::make_error_ok()) && (err != Marvin::make_error_ok() ) )
            m_first_err = err;
        tryDone();
    });
}
void TunnelHandler::tryDone()
{
    LogTrace("TryDone");
    if( m_upstream_done && m_downstream_done )
    {
        m_callback(m_first_err);
    }
}
