//
//  half_tunnel.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "half_tunnel.hpp"

HalfTunnel::HalfTunnel(ISocketSPtr readEnd, ISocketSPtr writeEnd)
{
    _readEnd = readEnd;
    _writeEnd = writeEnd;
    _bufferPtr = new MBuffer(20000);
    _bufferUPtr = std::unique_ptr<MBuffer>(_bufferPtr);
}
void HalfTunnel::start(std::function<void(Marvin::ErrorType& err)> cb)
{
    _callback = cb;
    startRead();
}
void HalfTunnel::startRead()
{
    auto hf = std::bind(&HalfTunnel::handleRead, this, std::placeholders::_1, std::placeholders::_2);
    _readEnd->asyncRead(*_bufferPtr, hf);
}
void HalfTunnel::handleRead(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
        auto hf = std::bind(&HalfTunnel::handleWrite, this, std::placeholders::_1, std::placeholders::_2);
        _writeEnd->asyncWrite(*_bufferPtr, hf);
    } else {
    }
}
void HalfTunnel::handleWrite(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    if( ! err ){
        startRead();
    } else {
    }
}
