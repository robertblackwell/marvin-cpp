
#ifndef HTTP_REQUEST_HANDLER_BASE_v2_HPP
#define HTTP_REQUEST_HANDLER_BASE_v2_HPP

#include <string>
#include <iostream>

#include <marvin/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server_v3/server_context.hpp>

namespace Marvin {

class RequestHandlerBase;

typedef std::shared_ptr<RequestHandlerBase> RequestHandlerBaseSPtr;
typedef std::unique_ptr<RequestHandlerBase> RequestHandlerBaseUPtr;

typedef std::function<RequestHandlerBase*(boost::asio::io_service& io)> RequestHandlerFactory;
typedef std::function<RequestHandlerBaseSPtr(boost::asio::io_service& io)> RequestHandlerSPtrFactory;
typedef std::function<RequestHandlerBaseUPtr(boost::asio::io_service& io)> RequestHandlerUPtrFactory;
typedef std::function<void()> HandlerDoneCallbackType;

class RequestHandlerBase
{
public:
    RequestHandlerBase(boost::asio::io_service& io);
    
    virtual ~RequestHandlerBase();
    
    virtual void handle(
        ServerContext&            server_context,
        ISocketSPtr                 clientConnectionSPtr,
        HandlerDoneCallbackType   done
    )
    {

    }
    
    protected:
        boost::asio::io_service&    m_io;
};

} // namespace Marvin

#endif // HTTP_REQUEST_HANDLER_INTERFACE_HPP
