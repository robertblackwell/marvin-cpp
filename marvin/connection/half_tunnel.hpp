//
//  half_tunnel.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef half_tunnel_hpp
#define half_tunnel_hpp

#include <stdio.h>
#include "bufferV2.hpp"
#include "connection_interface.hpp"

class HalfTunnel;
typedef std::shared_ptr<HalfTunnel> HalfTunnelSPtr;
typedef std::unique_ptr<HalfTunnel> HalfTunnelUPtr;


class HalfTunnel
{
    public:
        HalfTunnel(ConnectionInterfaceSPtr readEnd, ConnectionInterfaceSPtr writeEnd);
        void start(std::function<void(Marvin::ErrorType& err)> cb);
    private:
        void startRead();
        void handleRead(Marvin::ErrorType& err, std::size_t bytes_transfered);
        void handleWrite(Marvin::ErrorType& err, std::size_t bytes_transfered);
    
    
        ConnectionInterfaceSPtr     _readEnd;
        ConnectionInterfaceSPtr     _writeEnd;
        std::function<void(Marvin::ErrorType& err)> _callback;
        MBufferUPtr                 _bufferUPtr;
        MBuffer*                    _bufferPtr;
};

#endif /* half_tunnel_hpp */
