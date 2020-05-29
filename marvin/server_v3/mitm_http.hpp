#ifndef marvin_server_v3_http_mitm_hpp
#define marvin_server_v3_http_mitm_hpp

#include <marvin/server_v3/mitm_app.hpp>

namespace Marvin {
/**
* \ingroup mitmapp
*
* \brief Implements the http (no s) element of the mitm application. 
*
* A helper class which handles much of the the processing of a https request in the mitm_app
*/
class MitmHttp
{
public:

    MitmHttp(
        MitmApp& mitm_app,
        ISocketSPtr socket_sptr,
        MessageReaderV2::SPtr rdr,
        MessageWriter::SPtr wrtr,
        std::string scheme,
        std::string host,
        std::string port,
        ICollectorSPtr collector_sptr
    );
    ~MitmHttp();

    void handle();
private:

    void p_downstream_read_message();
    void p_initiate_upstream_roundtrip();
    void p_roundtrip_upstream(
        MessageBase::SPtr req,
        std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb);
    void p_on_request_completed();

    MitmApp&                    m_mitm_app;
    boost::asio::io_service&    m_io;
    ISocketSPtr                 m_downstream_socket_sptr;
    MessageReaderV2::SPtr       m_downstream_rdr_sptr;
    MessageWriterSPtr           m_downstream_wrtr_sptr;

    std::string                 m_scheme;
    std::string                 m_host;
    std::string                 m_port;
    MessageBase::SPtr           m_downstream_request_sptr;
    MessageBaseSPtr             m_downstream_response_sptr;

    std::string                 m_upstream_scheme;
    std::string                 m_upstream_host;
    std::string                 m_upstream_port;
    ISocketSPtr                 m_upstream_socket_sptr;
    MessageBase::SPtr           m_upstream_request_sptr;
    MessageBase::SPtr           m_upstream_response_sptr;
    BufferChain::SPtr           m_upstream_response_body_sptr;

    ::Cert::Certificate         m_server_certificate;
    ::Cert::Identity            m_mitm_identity;
    ClientSPtr                  m_upstream_client_uptr;
    ICollectorSPtr              m_collector_sptr;
};
} // namespace
#endif