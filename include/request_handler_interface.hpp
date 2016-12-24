
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
    //
    // This method is called on a CONNECT request to offer the opportunity to take over, or "hijack",
    // the connection for use that is independent of the server.
    //
    // By the time the method returns the parameters must be saved if they are required with
    // the following limitations:
    //
    //      io      -   will survive the scope of the call and can be used in other async operations
    //      req     -   will NOT survive beyond the call so dont use after the method returns, copy if required
    //      connPtr -   will NOT survive the call unless you copy it/save it. Its a smart pointer
    //
    //      if you want to take over the connection and not have the caller close the connection
    //      you must pass TRUE for (hijacked==true) to the callback. Passing false will
    //      result in the caller (server.cpp) closing the connection as soon as this method returns
    //
    //
    virtual void handleConnect(
        boost::asio::io_service&    io,
        MessageReader&              req,
        ConnectionPtr               connPtr,
        HandlerDoneCallbackType done)
        {done(false);}
    
    virtual void handleRequest(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& rep,
        HandlerDoneCallbackType done) = 0;
    
};
class TestHandler : public RequestHandlerInterface
{
public:
    void handleRequest(
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
