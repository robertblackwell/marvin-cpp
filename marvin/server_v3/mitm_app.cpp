#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>
#include <regex>
#include <marvin/server_v3/mitm_app.hpp>
#include <vector>
#include <marvin/http/message_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/helpers/mitm.hpp>
#include <marvin/server_v3/mitm_https.hpp>
#include <marvin/server_v3/mitm_http.hpp>
#include <marvin/server_v3/mitm_tunnel.hpp>

namespace Marvin {

enum class ConnectAction{
    TUNNEL=11,
    MITM,
    REJECT
};


std::vector<std::string> MitmApp::s_https_host_strings = {
    ".*hackernoon\\.com",
    ".*youtube.*",
    ".*geeksforgeeks.*",
    ".*bankofamerica.*",
    ".*github.*"
};

//void MitmApp::configSet_HttpsHosts(std::vector<std::regex> re)
//{
//    s_https_hosts = re;
//}
void MitmApp::configSet_HttpsHosts(std::vector<std::string> re_strings)
{
    s_https_host_strings = re_strings;
    std::vector<std::regex> regexes;
//    for( auto s: re_strings) {
//        regexes.push_back(std::regex(s));
//    }
//    s_https_hosts = regexes;
}
std::vector<std::string>& MitmApp::configGet_HttpsHosts()
{
    return s_https_host_strings;
}

//void MitmApp::configSet_HttpsPorts(std::vector<int> ports)
//{
//    s_https_ports = ports;
//}
//std::vector<int>& MitmApp::configGet_HttpsPorts()
//{
//    return s_https_ports;
//}
MitmApp::MitmApp(boost::asio::io_service& io, ICollectorSPtr collector_sptr, CaptureFilter::SPtr  filter_sptr): m_io(io)
{
   TROG_INFO("constructor");
   TROG_TRACE_CTOR();
//    m_https_hosts = s_https_hosts;
//    m_https_ports = s_https_ports;
    m_collector_sptr = collector_sptr;
    m_capture_filter_sptr = filter_sptr;
}


MitmApp::~MitmApp()
{
   TROG_TRACE_CTOR();
   TROG_INFO("destructor");
}

void MitmApp::handle(
    Marvin::ServerContext&          server_context,
    ISocketSPtr                     socket_sptr,
    Marvin::HandlerDoneCallbackType done
)
{
    m_socket_sptr = socket_sptr;
    m_rdr = std::make_shared<MessageReaderV2>(socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(socket_sptr);
    m_uuid = server_context.uuid;
    m_uuid_str = boost::uuids::to_string(m_uuid);
    m_done_callback = done;
    // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
    handle_request();
}
void MitmApp::p_on_completed()
{
    m_io.post(m_done_callback);
}

void MitmApp::handle_request()
{
    p_read_first_message();
}
void MitmApp::p_read_first_message()
{
    m_rdr->async_read_message([this](Marvin::ErrorType err)
    {
    if (err) {
        p_on_downstream_read_error(err);
    } else {
        m_request_msg_sptr = m_rdr->get_message_sptr();
        p_on_first_message();
    }
    });
}

void MitmApp::p_on_first_message()
{   
    std::string tmp_url = m_request_msg_sptr->target();
    // the Uri() constructor steals the input - I think this is fixed but better safe than sorry
    std::string tmp_url_safe = tmp_url;
    Uri tmp_uri = Uri(tmp_url_safe);

    m_scheme = tmp_uri.scheme();
    m_host = tmp_uri.host_no_port(); // tmp_uri.host() would have the port number on the end this is not what we want for a proxy
    m_port = std::to_string(tmp_uri.port());

    HttpMethod method = m_request_msg_sptr->method();
    auto sss = trace_message(*m_request_msg_sptr);
    // important logging point - if it breaks later in processing we need to be able to find out what was requested
    TROG_TRACE3("UUID: ", m_uuid_str, "FD: ", m_socket_sptr->native_socket_fd(), "HDRS: ", trace_message(*m_request_msg_sptr) );

    if (method == HttpMethod::CONNECT) {

        ConnectAction action = p_determine_action(m_host, m_port);

        switch(action){
            case ConnectAction::TUNNEL :
                m_mitm_tunnel_uptr = std::make_unique<MitmTunnel>(
                    *this,
                    m_socket_sptr,
                    m_rdr,
                    m_wrtr,
                    m_scheme,
                    m_host,
                    m_port
                );
                m_mitm_tunnel_uptr->handle();
                // p_initiate_tunnel();
                break;
            case ConnectAction::MITM :
                m_mitm_secure_uptr = std::make_unique<MitmHttps>(
                    *this,
                    m_socket_sptr,
                    m_rdr,
                    m_wrtr,
                    m_scheme,
                    m_host,
                    m_port,
                    m_collector_sptr
                );
                m_mitm_secure_uptr->handle();
                break;
            case ConnectAction::REJECT :
                assert(false);
                break;
        };

    } else {
        m_mitm_http_uptr = std::make_unique<MitmHttp>(
                    *this,
                    m_socket_sptr,
                    m_rdr,
                    m_wrtr,
                    m_scheme,
                    m_host,
                    m_port,
                    m_collector_sptr
                );
                
        m_mitm_http_uptr->handle();
        // p_initiate_http_upstream_roundtrip();
    }

};

void MitmApp::p_on_tunnel_completed()
{
    p_connection_end();
}
void MitmApp::p_connection_end()
{
    TROG_TRACE3("UUID: ", m_uuid, "FD: ", m_socket_sptr->native_socket_fd());
    p_on_completed(); // comes from the adapter
}

void MitmApp::p_on_downstream_write_error(Marvin::ErrorType& err)
{
    p_log_error("downstream write", err);
    p_connection_end();
}
void MitmApp::p_on_downstream_read_error(Marvin::ErrorType& err)
{
    p_log_error("downstream read", err);
    p_connection_end();
}
void MitmApp::p_on_upstream_error(Marvin::ErrorType& err)
{
    p_log_error("upstream", err);
    p_connection_end();
}
void MitmApp::p_on_upstream_roundtrip_error(Marvin::ErrorType& err)
{
    p_log_error("Tunnel", err);
    p_connection_end();
}
void MitmApp::p_on_tunnel_error(Marvin::ErrorType& err)
{
    p_log_error("Tunnel", err);
    p_on_tunnel_completed();
}
void MitmApp::p_log_error(std::string label, Marvin::ErrorType err)
{
    TROG_TRACE3("UUID: ", m_uuid, "FD: ", m_socket_sptr->native_socket_fd());
}
/**
 * Determine what to do about a CONNECT request. 
 * This function will return "tunnel" as the action unless
 * the host name one of the pattersns in the mitm_host vector
 * AND the port is 443
 * Connects on port 80 are possibly UPGRADE requests for a websocket connection
 * but that wont be know for sure until the next aprt of the exchange
 * with the server.
 * On the other side if the Connect::443 is start of a websocket upgrade request
 * that will be aborted (or maybe handled in a future release) by the mitm_https
 * module and the correct reaction is to remove that target host from the mimt_host
 * vector 
 */
ConnectAction MitmApp::p_determine_action(std::string host, std::string port)
{

    if (m_capture_filter_sptr->is_collectable(host)) {
        return ConnectAction::MITM;
    }
    return ConnectAction::TUNNEL;
}

} // namespace Marvin
