
#include <iostream>
#include <sstream>
#include <string>
#include <pthread.h>

#include <marvin/boost_stuff.hpp>
#include <json/json.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/http/http_header.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/external_src/CxxUrl/url.hpp>
#include <marvin/http/uri_query.hpp>

#include <marvin/server_v2/http_server_v2.hpp>
#include <marvin/server_v2/request_handler_base_v2.hpp>

#include "v2_handler.hpp"

using namespace Marvin;
using namespace Http;

MessageBaseSPtr make_200_response(std::string body)
{
    MessageBaseSPtr msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setIsRequest(false);
    msg->setStatusCode(200);
    msg->setStatus("OK");
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);

    Marvin::BufferChainSPtr bchain_sptr = Marvin::BufferChain::makeSPtr(body);
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(body.length() ));
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
    msg->setHeader(Marvin::Http::Headers::Name::ContentLength, std::to_string(body.length() ));
    return msg;
}
V2Handler::V2Handler(boost::asio::io_service& io): Marvin::RequestHandlerBaseV2(io)
{

}
V2Handler::~V2Handler()
{
}

void V2Handler::handle(
    Marvin::ServerContextV2&   server_context,
    ISocketSPtr                 socket_sptr,
    Marvin::HandlerDoneCallbackTypeV2 done
)
{
    MessageReaderSPtr rdr = std::make_shared<MessageReader>(m_io, socket_sptr);
    rdr->readMessage([socket_sptr, done](Marvin::ErrorType err){

        std::string resp = "HTTP/1.1 200 OK\r\nContent-length: 18\r\nContent-type: plaint\text\r\n\r\nTHIS_IS_A_RESPONSE\r\n\r\n";
        socket_sptr->asyncWrite(resp, [done](Marvin::ErrorType& err, std::size_t bytes_transfered) {
            std::cout << __PRETTY_FUNCTION__ << "" << std::endl;
            done();
        });
    });
    // MessageBaseSPtr response_msg = make_200_response("This is a response message");
    // MessageWriterSPtr wrtr = std::make_shared<MessageWriter>(m_io, connPtr);
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    // wrtr->asyncWrite(response_msg, [done](Marvin::ErrorType& err) {
    //     done();
    // });

    /**
    * Let the sub handlers do their own response as we sometimes want those
    * sub handlers to do other async operations
    */
}
