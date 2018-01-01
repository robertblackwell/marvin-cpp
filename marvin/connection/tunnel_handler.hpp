
#ifndef marvin_tunnel_handler_hpp
#define marvin_tunnel_handler_hpp

#include <stdio.h>
#include <memory>
#include "marvin_error.hpp"
#include "tcp_connection.hpp"
#include "half_tunnel.hpp"

class TunnelHandler;
/// \ingroup SocketIO
using TunnelHandlerSPtr = std::shared_ptr<TunnelHandler> ;
/// \ingroup SocketIO
using TunnelHandlerUPtr = std::unique_ptr<TunnelHandler>;

/**
* \ingroup SocketIO
* \brief Cmbines two HalfTunnel objects to provide bi directional pipe between two end points.
*/
class TunnelHandler
{
    public:
        TunnelHandler(
            ISocketSPtr            downStreamConnection,
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
