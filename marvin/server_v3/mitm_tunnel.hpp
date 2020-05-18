#ifndef marvin_server_v3_mitm_tunnel_hpp
#define marvin_server_v3_mitm_tunnel_hpp

#include <marvin/buffer/buffer.hpp>
#include <marvin/server_v3/mitm_app.hpp>

namespace Marvin {
/** 
* \ingroup mitmapp
*
* \brief Implements the https tunnel function.
*/
    class MitmTunnel
{
public:
    using BufferStrategy = TunnelAllocator;
    MitmTunnel(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderSPtr rdr,
        MessageWriterSPtr wrtr,
        std::string scheme,
        std::string host,
        std::string port
    );
    ~MitmTunnel();

    void handle();
private:

    void p_downstream_read_message();
    void p_initiate_tunnel();
    void p_on_request_completed();

    MitmApp&                    m_mitm_app;
    boost::asio::io_service&    m_io;
    BufferStrategy              m_strategy;
    ContigBufferFactoryT        m_factory;
    ISocketSPtr                 m_downstream_socket_sptr;
    MessageReaderSPtr           m_downstream_rdr_sptr;
    MessageWriterSPtr           m_downstream_wrtr_sptr;
    // std::string                 m_scheme;
    // std::string                 m_host;
    // std::string                 m_port;
    MessageBaseSPtr             m_downstream_response_sptr;
    std::string                 m_upstream_scheme;
    std::string                 m_upstream_host;
    std::string                 m_upstream_port;
    ISocketSPtr                 m_upstream_connection_sptr;
    // MessageBaseSPtr             m_upstream_request_sptr;
            // obvious - tunnel handler
    TunnelHandlerSPtr                   m_tunnel_handler_sptr;

};
} // namespace
#endif