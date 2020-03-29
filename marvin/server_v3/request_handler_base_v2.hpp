
#ifndef HTTP_REQUEST_HANDLER_BASE_v2_HPP
#define HTTP_REQUEST_HANDLER_BASE_v2_HPP

#include <string>
#include <iostream>

#include <marvin/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server_v2/server_context_v2.hpp>

namespace Marvin {

class RequestHandlerBaseV2;

typedef std::shared_ptr<RequestHandlerBaseV2> RequestHandlerBaseV2SPtr;
typedef std::unique_ptr<RequestHandlerBaseV2> RequestHandlerBaseV2UPtr;

typedef std::function<RequestHandlerBaseV2*(boost::asio::io_service& io)> RequestHandlerFactoryV2;
typedef std::function<RequestHandlerBaseV2SPtr(boost::asio::io_service& io)> RequestHandlerSPtrFactoryV2;
typedef std::function<RequestHandlerBaseV2UPtr(boost::asio::io_service& io)> RequestHandlerUPtrFactoryV2;
typedef std::function<void()> HandlerDoneCallbackTypeV2;

class RequestHandlerBaseV2
{
public:
    RequestHandlerBaseV2(boost::asio::io_service& io);
    
    virtual ~RequestHandlerBaseV2();
    
    virtual void handle(
        ServerContextV2&            server_context,
        ISocketSPtr                 clientConnectionSPtr,
        HandlerDoneCallbackTypeV2   done
    )
    {

    }
    
    protected:
        boost::asio::io_service&    m_io;
};

} // namespace Marvin

#endif // HTTP_REQUEST_HANDLER_INTERFACE_HPP
