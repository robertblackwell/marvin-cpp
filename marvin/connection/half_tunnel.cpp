//
//  half_tunnel.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "half_tunnel.hpp"

HalfTunnel::HalfTunnel(ISocketSPtr readEnd, ISocketSPtr writeEnd)
{
    m_read_end = readEnd;
    m_write_end = writeEnd;
    m_bufferSPtr = Marvin::MBuffer::makeSPtr(20000);
//    m_bufferUPtr = std::unique_ptr<Marvin::MBuffer>(m_bufferPtr);
}
void HalfTunnel::start(std::function<void(Marvin::ErrorType& err)> cb)
{
    m_callback = cb;
    p_start_read();
}
void HalfTunnel::p_start_read()
{
    auto hf = std::bind(&HalfTunnel::p_handle_read, this, std::placeholders::_1, std::placeholders::_2);
    m_read_end->asyncRead(m_bufferSPtr, hf);
}
void HalfTunnel::p_handle_read(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
        LogTrace("OK Read");
        auto hf = std::bind(&HalfTunnel::p_handle_write, this, std::placeholders::_1, std::placeholders::_2);
        m_write_end->asyncWrite(*m_bufferSPtr, hf);
    } else {
        std::string m = Marvin::make_error_description(err);
        m_callback(err);
//        assert(false);
    }
}
void HalfTunnel::p_handle_write(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
        LogTrace("OK write");
        p_start_read();
    } else {
        std::string m = Marvin::make_error_description(err);
        m_callback(err);
//        assert(false);
    }
}
