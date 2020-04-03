
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <json/json.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/external_src/CxxUrl/url.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v3/http_server.hpp>
#include <marvin/server_v3/request_handler_base.hpp>

#include "handler.hpp"

using namespace Marvin;
using namespace Http;

Handler::Handler(boost::asio::io_service& io): Marvin::RequestHandlerBase(io)
{

}
Handler::~Handler()
{
}

void Handler::handle(
    Marvin::ServerContext&          server_context,
    ISocketSPtr                     socket_sptr,
    Marvin::HandlerDoneCallbackType done
)
{
    m_socket_sptr = socket_sptr;
    m_rdr = std::make_shared<MessageReader>(m_io, socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(m_io, socket_sptr);
    m_done_callback = done;
    // handleRequest(m_socket_sptr, m_wrtr, m_rdr);
    handleRequest();
}
/// determine whether to callback to the server or start another read/write cycle
void Handler::p_req_resp_cycle_complete()
{
    // assume all connections are persistent
    LogWarn("Handler::p_req_resp_cycle_complete");
    bool keep_alive = false;
    /// @TODO - this is a hack
    if (m_rdr->hasHeader(Marvin::Http::HeadersV2::Connection)) {
        std::string conhdr = m_rdr->getHeader(Marvin::Http::HeadersV2::Connection);
        keep_alive = (conhdr == "Keep-Alive");
    }
    if (keep_alive) {
        // handleRequest(m_socket_sptr, m_wrtr, m_rdr);
        handleRequest();
    } else {
        m_socket_sptr->shutdown(ISocket::ShutdownSend); // remember this is actually shutdown send side
        m_done_callback();
    }
    // m_done_callback();
}
void Handler::p_on_read_error(Marvin::ErrorType err)
{
    LogWarn("Handler p_on_read_error : ", err.message());
    // m_socket_sptr->close();
    m_done_callback();
}
void Handler::p_on_write_error(Marvin::ErrorType err)
{
    LogWarn("Handler p_on_write_error : ", err.message());
    // m_socket_sptr->close();
    m_done_callback();
}