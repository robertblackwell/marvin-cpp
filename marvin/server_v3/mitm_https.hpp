#ifndef marvin_server_v3_mitm_https_hpp
#define marvin_server_v3_mitm_https_hpp

#include <marvin/server_v3/mitm_app.hpp>

namespace Marvin {

/**
* \ingroup mitmapp
*
* \brief IMplements the https element of the mitm application. 
*
* A helper class which handles much of the the processing of a https request in the mitm_app
*/

class MitmHttps
{
public:

    MitmHttps(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderSPtr rdr,
        MessageWriterSPtr wrtr,
        std::string scheme,
        std::string host,
        std::string port,
        ICollectorSPtr collector_sptr
    );
    ~MitmHttps();

    void handle();
private:
    void p_handshake_upstream();
    void p_downstream_read_message();
    void p_initiate_upstream_roundtrip();
    void p_roundtrip_upstream(
        MessageReaderSPtr req,
        std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb);
    void p_on_upstream_connect_handshake_error(ErrorType& err);
    void p_on_request_completed();

    MitmApp&                    m_mitm_app;
    boost::asio::io_service&    m_io;
    ISocketSPtr                 m_downstream_socket_sptr;
    MessageReaderSPtr           m_downstream_rdr_sptr;
    MessageWriterSPtr           m_downstream_wrtr_sptr;
    std::string                 m_scheme;
    std::string                 m_host;
    std::string                 m_port;
    MessageBaseSPtr             m_downstream_response_sptr;
    std::string                 m_upstream_scheme;
    std::string                 m_upstream_host;
    std::string                 m_upstream_port;
    ISocketSPtr                 m_upstream_socket_sptr;
    MessageBaseSPtr             m_upstream_request_sptr;
    BufferChainSPtr             m_upstream_response_body_sptr;
    ::Cert::Certificate         m_server_certificate;
    ::Cert::Identity            m_mitm_identity;
    ClientSPtr                  m_upstream_client_uptr;
    ICollectorSPtr              m_collector_sptr;
};
} // namespace
#endif