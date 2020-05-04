
#ifndef marvin_connection_half_tunnel_hpp
#define marvin_connection_half_tunnel_hpp

#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>

namespace Marvin {

class HalfTunnel;
/// \ingroup SocketIO
 using HalfTunnelSPtr = std::shared_ptr<HalfTunnel>;
/// \ingroup SocketIO
 using HalfTunnelUPtr = std::unique_ptr<HalfTunnel>;

/**
* \ingroup connection
* \brief Provides a single direction pipe between two sockets and streams data until eof; meant to be used in
* pairs to provide bi-dirctional streaming between two socket endpoints
*/
class HalfTunnel
{
    public:
        HalfTunnel(ISocketSPtr readEnd, ISocketSPtr writeEnd, long firstReadTimeoutMillisecs, long subsequentReadTimeoutMillisecs);
        void start(std::function<void(Marvin::ErrorType& err)> cb);
    protected:
        void p_start_read();
        void p_handle_read(Marvin::ErrorType& err, std::size_t bytes_transfered);
        void p_handle_write(Marvin::ErrorType& err, std::size_t bytes_transfered);
    
        long            m_first_read_timeout_millisecs;
        long            m_subsequent_read_timeout_millisecs;
        ISocketSPtr     m_read_end;
        ISocketSPtr     m_write_end;
        std::function<void(Marvin::ErrorType& err)> m_callback;
//        Marvin::MBufferUPtr                 m_bufferUPtr;
        Marvin::MBufferSPtr                 m_bufferSPtr;
};
} // namespace
#endif /* half_tunnel_hpp */
