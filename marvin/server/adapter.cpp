#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>
#include <marvin/server/adapter.hpp>


#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <json/json.hpp>

#include <marvin/http/header_fields.hpp>
#include <marvin/http/message_base.hpp>
#include<uri-parser/UriParser.hpp>
#include<CxxUrl/url.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_interface.hpp>

using namespace Marvin;
using namespace Http;

Marvin::Adapter::Adapter(boost::asio::io_service& io): m_io(io)
{

}
Marvin::Adapter::~Adapter()
{
}

void Marvin::Adapter::handle(
    Marvin::ServerContext&          server_context,
    ISocketSPtr                     socket_sptr,
    Marvin::HandlerDoneCallbackType done
)
{
    m_socket_sptr = socket_sptr;
    m_rdr = std::make_shared<MessageReader>(socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(socket_sptr);
    m_done_callback = done;
    // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
    handleRequest();
}
void Marvin::Adapter::p_on_completed()
{
    p_req_resp_cycle_complete();
}
/// determine whether to callback to the server or start another read/write cycle
void Marvin::Adapter::p_req_resp_cycle_complete()
{
    // assume all connections are persistent
    TROG_WARN("Adapter::p_req_resp_cycle_complete");
    bool keep_alive = false;
    /// @TODO - this is a hack
    auto hopt = m_rdr->header(Marvin::HeadersV2::Connection);
    if (hopt) {
        std::string conhdr = hopt.get();
        keep_alive = (conhdr == "Keep-Alive");
    }
    if (keep_alive) {
        // Adapterequest(m_socket_sptr, m_wrtr, m_rdr);
        handleRequest();
    } else {
        m_socket_sptr->shutdown(ISocket::ShutdownSend); // remember this is actually shutdown send side
        m_done_callback();
    }
    // m_done_callback();
}
void Marvin::Adapter::p_on_read_error(Marvin::ErrorType err)
{
    TROG_WARN("Adapter p_on_read_error : ", err.message());
    // m_socket_sptr->close();
    m_done_callback();
}
void Marvin::Adapter::p_on_write_error(Marvin::ErrorType err)
{
    TROG_WARN("Adapter p_on_write_error : ", err.message());
    // m_socket_sptr->close();
    m_done_callback();
}