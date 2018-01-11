
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
        std::function<void(Marvin::ErrorType& err)> m_callback;

        ISocketSPtr             m_downstream_connection;
        ISocketSPtr             m_upstream_connection;
    
        HalfTunnelUPtr          m_upstream_halftunnel;
        HalfTunnelUPtr          m_downstream_halftunnel;
    
        bool                    m_upstream_done;
        Marvin::ErrorType       m_upstream_err;
        bool                    m_downstream_done;
        Marvin::ErrorType       m_downstream_err;
        Marvin::ErrorType       m_first_err;
};

#endif /* tunnel_handler_hpp */
