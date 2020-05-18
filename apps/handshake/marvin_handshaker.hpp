#ifndef marvin_handshaker_hpp
#define marvin_handshaker_hpp
#include <string>
#include <marvin/error/marvin_error.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>

namespace Marvin {
/** A helper class which handles much of the the processing of a https request in the mitm_app*/
class Handshaker
{
public:

    Handshaker(boost::asio::io_service& io, std::string host);
    ~Handshaker();

    void handshake(std::function<void(ErrorType err)> cb);
    Cert::Certificate getServerCertificate();

private:
    void p_handshake_upstream();
    void p_on_handshake_error(ErrorType& err);
    void p_on_handshake_ok();

    void p_on_connect_error(ErrorType& err);
    void p_on_request_completed();

    boost::asio::io_service&    m_io;
    std::string                 m_scheme;
    std::string                 m_host;
    std::string                 m_port;
    std::function<void(ErrorType err)> m_callback;
    ISocketSPtr                 m_socket_sptr;
    MessageBaseSPtr             m_upstream_request_sptr;
    BufferChain::SPtr             m_upstream_response_body_sptr;
    ::Cert::Certificate         m_server_certificate;
    ::Cert::Identity            m_mitm_identity;

};
} // namespace
#endif