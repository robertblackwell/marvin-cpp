
#ifndef HTTP_REQUEST_HANDLER_INTERFACE_HPP
#define HTTP_REQUEST_HANDLER_INTERFACE_HPP

#include <string>
#include <iostream>
#include "boost/asio.hpp"
#include "message_reader.hpp"
#include "message_writer.hpp"

typedef std::function<void(bool)> HandlerDoneCallbackType;

class RequestHandlerInterface
{
public:
    
    virtual void handle_request(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& rep,
        HandlerDoneCallbackType done) = 0;
    
};
class TestHandler : public RequestHandlerInterface
{
public:
    void handle_request(
        boost::asio::io_service io,
        MessageReader& req,
        MessageWriter& rep,
        HandlerDoneCallbackType done){
//        std::cout << "got to a new handler " << std::endl;
//        rep = reply::stock_reply(reply::ok_just_joking);
//        rep.status = reply::ok;
//        rep.content = "<html><body>hello world</body></html>";
//        rep.headers.resize(1);
//        rep.headers[0].name = "Content-Length";
//        rep.headers[0].value = std::to_string(rep.content.size());
//        done(true);
    }
};

#endif // HTTP_REQUEST_HANDLER_INTERFACE_HPP
