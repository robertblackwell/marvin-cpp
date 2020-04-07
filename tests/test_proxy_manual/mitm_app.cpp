#include "mitm_app.hpp"

#include <marvin/http/message_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

namespace Marvin {

enum class ConnectAction{
    TUNNEL=11,
    MITM,
    REJECT
};


std::vector<std::regex> MitmApp::s_https_hosts = std::vector<std::regex>();
std::vector<int> MitmApp::s_https_ports = std::vector<int>();

void MitmApp::configSet_HttpsHosts(std::vector<std::regex> re)
{
    s_https_hosts = re;
}


void MitmApp::configSet_HttpsPorts(std::vector<int> ports)
{
    s_https_ports = ports;
}

MitmApp::MitmApp(boost::asio::io_service& io, ICollectorSPtr collector_sptr): m_io(io)
{
    LogTorTrace();
    m_https_hosts = s_https_hosts;
    m_https_ports = s_https_ports;
    m_collector_sptr = collector_sptr;
}


MitmApp::~MitmApp()
{
    LogTorTrace();
}

void MitmApp::handle(
    Marvin::ServerContext&          server_context,
    ISocketSPtr                     socket_sptr,
    Marvin::HandlerDoneCallbackType done
)
{
    m_socket_sptr = socket_sptr;
    m_rdr = std::make_shared<MessageReader>(m_io, socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(m_io, socket_sptr);
    m_done_callback = done;
    // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
    handleRequest();
}
void MitmApp::p_on_completed()
{
    m_io.post(m_done_callback);
}

void MitmApp::p_handle_upgrade()
{
}

void MitmApp::handleRequest()
{
    p_read_first_message();
}
void MitmApp::p_read_first_message()
{
    m_rdr->readMessage([this](Marvin::ErrorType err) 
    {
        if (err) {
            p_on_downstream_read_error(err);
        } else {
            p_on_first_message();
        }
    });
}
void MitmApp::p_read_another_message()
{
    // maybe do something about shortening the read timeout so we dont wait on a client 
    // that has gone away
    p_read_first_message();
}
void MitmApp::p_on_first_message()
{   
    std::string tmp_url = m_rdr->uri();
    std::string tmp_url_safe = tmp_url;
    Uri tmp_uri = Uri(tmp_url_safe);

    m_scheme = tmp_uri.scheme();
    m_host = tmp_uri.server(); // tmp_uri.host() would have the port number on the end this is not what we want for a proxy
    m_port = std::to_string(tmp_uri.port());

    HttpMethod method = m_rdr->method();

    if (method == HttpMethod::CONNECT) {

        ConnectAction action = p_determine_action(m_host, m_port);

        switch(action){
            case ConnectAction::TUNNEL :
                p_initiate_tunnel();
                break;
            case ConnectAction::MITM :
                p_initiate_https_upstream_roundtrip();
                break;
            case ConnectAction::REJECT :
                assert(false);
                break;
        };

    } else {
        p_initiate_http_upstream_roundtrip();
    }

};

void MitmApp::p_initiate_tunnel()
{
    LogTrace("scheme:", m_scheme, " host:", m_host, " port:", m_port);

    m_upstream_connection_sptr = socketFactory(m_io, m_scheme, m_host, m_port);
    m_upstream_connection_sptr->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn){
        if( err ) {
            LogWarn("initiateTunnel: FAILED scheme:", this->m_scheme, " host:", this->m_host, " port:", this->m_port);
            m_downstream_response_sptr = std::make_shared<MessageBase>();
            makeResponse502Badgateway(*m_downstream_response_sptr);

            m_wrtr->asyncWrite(m_downstream_response_sptr, [this](Marvin::ErrorType& err){
                LogInfo("");
                if( err ) {
                    LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    p_on_downstream_write_error(err);
                } else {
                    p_on_tunnel_completed();
                }
            });
        } else {
            LogTrace("initiateTunnel: connection SUCCEEDED scheme:", " scheme:",this->m_scheme, " host:", this->m_host, " port:", this->m_port);
            m_downstream_response_sptr = std::make_shared<MessageBase>();
            makeResponse200OKConnected(*m_downstream_response_sptr);
            m_wrtr->asyncWrite(m_downstream_response_sptr, [this](Marvin::ErrorType& err){
                LogInfo("");
                if( err ) {
                    LogWarn("error: ", err.value(), err.category().name(), err.category().message(err.value()));
                    p_on_downstream_write_error(err);
                } else {
                    m_tunnel_handler_sptr = std::make_shared<TunnelHandler>(m_io, m_socket_sptr, m_upstream_connection_sptr);
                    m_tunnel_handler_sptr->start([this](Marvin::ErrorType& err) {
                        if (err) {
                            p_on_tunnel_error(err);
                        } else {
                            p_on_tunnel_completed();
                        }
                    });
                }
            });
        }
    });
}
void MitmApp::p_initiate_https_upstream_roundtrip()
{

}
void MitmApp::p_initiate_http_upstream_roundtrip()
{
   
    Marvin::Uri tmp_uri(m_rdr->uri());
    assert( ! m_rdr->hasHeader("Upgrade") );
    p_roundtrip_upstream(m_rdr, [this](MessageBaseSPtr downMsg){
        /// get here with a message suitable for transmission to down stream client
        m_downstream_response_sptr = downMsg;
        LogTrace("for downstream", traceMessage(*downMsg));
        Marvin::BufferChainSPtr responseBodySPtr = downMsg->getContentBuffer();
        /// perform the MITM collection
        
        m_collector_sptr->collect(m_scheme, m_host, m_rdr, m_downstream_response_sptr);
        
        /// write response to downstream client
        m_wrtr->asyncWrite(m_downstream_response_sptr, responseBodySPtr, [this](Marvin::ErrorType& err) {
            if (err) {
                p_on_downstream_write_error(err);
            } else {
                p_on_request_completed();
            }
        });

    });
}
/// \brief Perform the proxy forwarding process; and produces a response suitable
/// for downstream transmission; the result of this method is a response to send back to the client
/// \param req : MessageReaderSPtr the request from the original client - has same value
///                                 as class property m_request_sptr
/// \param upstreamCb : called when the round trip has finished
///
void MitmApp::p_roundtrip_upstream(
        MessageReaderSPtr req,
        std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb
){
    /// a client object to manage the round trip of request and response to
    /// the final destination. m_host m_scheme and m_port already setup

    m_upstream_client_uptr = std::unique_ptr<Client>(new Client(m_io, m_scheme, m_host, m_port));
    /// the MessageBase that will be the up stream request
    m_upstream_request_sptr = std::make_shared<MessageBase>();
    /// format upstream msg for transmission
    Helpers::makeUpstreamRequest(m_upstream_request_sptr, req);
    assert( ! m_rdr->hasHeader("Upgrade") );
    Marvin::BufferChainSPtr content = req->getContentBuffer();
    
    m_upstream_client_uptr->asyncWrite(m_upstream_request_sptr, content, [this, upstreamCb](Marvin::ErrorType& ec, MessageReaderSPtr upstrmRdr)
    {
        if (ec || (upstrmRdr == nullptr)) {
            std::string desc = make_error_description(ec);
            LogWarn("async write failed ", make_error_description(ec));
            p_on_upstream_roundtrip_error(ec);
            // TODO: how to handle error
        } else {
            LogTrace("upstream rresponse", traceMessage(*(upstrmRdr.get())));
            m_downstream_response_sptr = std::make_shared<MessageBase>();
            m_upstream_response_body_sptr = upstrmRdr->getContentBuffer();
            Helpers::makeDownstreamResponse(m_downstream_response_sptr, upstrmRdr, ec);
            upstreamCb(m_downstream_response_sptr);
        }
    });
    
};
void MitmApp::p_on_tunnel_completed()
{
    p_connection_end();
}
void MitmApp::p_on_request_completed()
{
    if ((isConnectionKeepAlive(*m_rdr) && isConnectionKeepAlive(*m_downstream_response_sptr))) {
        p_read_another_message();
    } else {
        p_connection_end();
    }

}
void MitmApp::p_connection_end()
{
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

}

ConnectAction MitmApp::p_determine_action(std::string host, std::string port)
{
    std::vector<std::regex>  regexs = this->m_https_hosts;
    std::vector<int>         ports  = this->m_https_ports;
    /// !!! this needs to be upgraded
    if (std::stoi(port) == 443) {
        return ConnectAction::MITM;
    }
    return ConnectAction::TUNNEL;
}

} // namespace Marvin
