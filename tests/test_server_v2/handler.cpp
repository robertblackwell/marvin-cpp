
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <json/json.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/external_src/CxxUrl/url.hpp>

#include <marvin/server_v2/http_server.hpp>
#include <marvin/server_v2/request_handler_base.hpp>

#include <marvin/http/uri_query.hpp>
#include "./handler.hpp"

using namespace Marvin;
using namespace Http;

nlohmann::json jsonize_request(V2Handler* req_handler, V2::ServerContext& server_context, MessageReaderSPtr req)
{
    Marvin::BufferChainSPtr bsp = req->getContentBuffer();
    std::string bodyString = bsp->to_string();
    nlohmann::json j;
    std::vector<std::pair<std::string, std::string>> jz = (req->getHeaders()).jsonizable();
//    nlohmann::json jj{jz};
    j["req"] = {
        {"method", req->getMethodAsString()},
        {"uri", req->uri()},
        {"headers", (req->getHeaders())},
        {"body",bodyString}
    };
    j["xtra"] = {
        {"connection_handler_uuid", server_context.connection_handler_ptr->uuid()},
        {"request_handler_uuid", boost::uuids::to_string(req_handler->_uuid)},
        {"fd", server_context.connection_ptr->nativeSocketFD()}
    };
    return j;
}
MessageBaseSPtr make_200_response(std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(200);
    msg->setStatus("OK");
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    msg->setHeader(Marvin::Http::HeadersV2::ContentLength, std::to_string(body.length() ));
    return msg;
}
MessageBaseSPtr make_response(int status_code, std::string status, std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(status_code);
    msg->setStatus(status);
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    msg->setHeader(Marvin::Http::HeadersV2::ContentLength, std::to_string(body.length() ));
    return msg;
}

bool apply_connection_close(MessageReaderSPtr req, MessageBaseSPtr response)
{
    response->setHeader(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
    return false;
}

bool apply_keepalive_rules(MessageReaderSPtr req, MessageBaseSPtr response)
{
    /// correctly handle keep-alive/close
    bool keep_alive;
    if(req->getHeader(Marvin::Http::HeadersV2::Connection) == Marvin::Http::HeadersV2::ConnectionKeepAlive) {
        keep_alive = true;
        response->setHeader(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionKeepAlive);
    } else {
        keep_alive = false;
        response->setHeader(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
    }
    return keep_alive;
}

int V2Handler::counter;
V2Handler::V2Handler(boost::asio::io_service& io):RequestHandlerBase(io), _timer(io), _uuid(boost::uuids::random_generator()())
{
    counter++;
}
V2Handler::~V2Handler()
{
    LogDebug("");
}

void V2Handler::handleConnect(
    V2::ServerContext&              server_context,
    MessageReaderSPtr           req,
    ISocketSPtr     connPtr,
    V2::HandlerDoneCallbackType     done)
{
    LogDebug("");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
    auto er = Marvin::make_error_ok();
#pragma cland diagnostic pop
    boost::asio::streambuf b;
    std::ostream strm(&b);
    strm << "HTTP/1.1 200 OK\r\nContent-length:5\r\n\r\n12345" << std::endl;
    connPtr->asyncWrite(b, [this, done, &connPtr](Marvin::ErrorType& err, std::size_t bytes_transfered){
        auto er = Marvin::make_error_ok();
        LogDebug(" callback");
        done(er, true);
    });
}
void V2Handler::handleRequest(
    V2::ServerContext&   server_context,
    boost::asio::io_service& io,
    MessageReaderSPtr req,
    MessageWriterSPtr resp,
    ISocketSPtr     socket_sptr,
    V2::HandlerDoneCallbackType done
)
{
    MessageBaseSPtr msg;
    std::string body;
    bool keep_alive;
    /**
    * Let the sub handlers do their own response as we sometimes want those
    * sub handlers to do other async operations
    */
    #if 0
    Marvin::MBufferSPtr mbuf = Marvin::MBuffer::makeSPtr(10000);
    socket_sptr->asyncRead(mbuf, [mbuf](Marvin::ErrorType& err, std::size_t bytes)
    {
        std::string s = mbuf->toString();
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    });
    return;
    #endif
    m_rdr = std::make_shared<MessageReader>(io, socket_sptr);
    m_wrtr = std::make_shared<MessageWriter>(io, socket_sptr);
    m_rdr->readMessage([this, &server_context, req, resp, done](Marvin::ErrorType err )
    {
        // handle_get(server_context, req, resp, done);
        std::string body;
        body = "This is a response just to see we got one";
        MessageBaseSPtr msg = make_200_response(body);
        auto txt = msg->str();
        msg->setHeader(Marvin::Http::HeadersV2::Connection, Marvin::Http::HeadersV2::ConnectionClose);
        m_wrtr->asyncWrite(msg, body, [this, done](Marvin::ErrorType& err)
        {
            done(err, false);
        });
    });

}
