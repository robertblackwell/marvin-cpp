
#ifndef marvin_connection_tunnel_handler_hpp
#define marvin_connection_tunnel_handler_hpp

#include <stdio.h>
#include <memory>
#include <marvin/connection/half_tunnel.hpp>
#include <marvin/error/marvin_error.hpp>

namespace Marvin {

class TunnelHandler;
using TunnelHandlerSPtr = std::shared_ptr<TunnelHandler> ;
using TunnelHandlerUPtr = std::unique_ptr<TunnelHandler>;
std::string trace_tunnel(TunnelHandlerUPtr);
std::string trace_tunnel(TunnelHandlerSPtr);
/**
* \ingroup connection
* \brief Combines two HalfTunnel objects to provide bi directional pipe between two end points.
*/
class TunnelHandler
{
    public:
        using SPtr = std::shared_ptr<TunnelHandler>;
        using UPtr = std::unique_ptr<TunnelHandler>;
        TunnelHandler(
            boost::asio::io_service&    io,
            ISocketSPtr                 downStreamConnection,
            ISocketSPtr                 upstreamConnection
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
        boost::asio::io_service& m_io;
        bool                    m_upstream_done;
        Marvin::ErrorType       m_upstream_err;
        bool                    m_downstream_done;
        Marvin::ErrorType       m_downstream_err;
        Marvin::ErrorType       m_first_err;
        long                    m_first_read_timeout_millisecs;
        long                    m_subsequent_read_timeout_millisecs;
};
} // namespace Marvin
#endif /* tunnel_handler_hpp */
