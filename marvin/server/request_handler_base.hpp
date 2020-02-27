
#ifndef HTTP_REQUEST_HANDLER_BASE_HPP
#define HTTP_REQUEST_HANDLER_BASE_HPP

#include <string>
#include <iostream>

#include <marvin/include/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server/server_context.hpp>

class RequestHandlerBase;

typedef std::shared_ptr<RequestHandlerBase> RequestHandlerBaseSPtr;
typedef std::unique_ptr<RequestHandlerBase> RequestHandlerBaseUPtr;

typedef std::function<RequestHandlerBase*(boost::asio::io_service& io)> RequestHandlerFactory;
typedef std::function<RequestHandlerBaseSPtr(boost::asio::io_service& io)> RequestHandlerSPtrFactory;
typedef std::function<RequestHandlerBaseUPtr(boost::asio::io_service& io)> RequestHandlerUPtrFactory;

typedef std::function<void(Marvin::ErrorType& err, bool keepAlive)> HandlerDoneCallbackType;
typedef std::function<void(bool hijackConnectioin)> ConnectHandlerHijackCallbackType;

class RequestHandlerBase
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
    RequestHandlerBase(boost::asio::io_service& io);
    
    virtual ~RequestHandlerBase();
    
    virtual void handleConnect(
        ServerContext&              server_context,
        MessageReaderSPtr           request,
        MessageWriterSPtr           repsponseWriter,
        ISocketSPtr                 clientConnectionSPtr,
        HandlerDoneCallbackType     done)
        { auto err = Marvin::make_error_ok(); done(err,false);}
    
    virtual void handleRequest(
        ServerContext&              server_context,
        MessageReaderSPtr           request,
        MessageWriterSPtr           responseWriter,
        HandlerDoneCallbackType done) = 0;
    
    protected:
        boost::asio::io_service&    m_io;
};



#endif // HTTP_REQUEST_HANDLER_INTERFACE_HPP
