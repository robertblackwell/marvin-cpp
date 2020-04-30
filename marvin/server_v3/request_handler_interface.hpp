
#ifndef marvin_server_v3_request_handler_interface_hpp
#define marvin_server_v3_request_handler_interface_hpp

#include <string>
#include <iostream>

#include <marvin/boost_stuff.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/server_v3/server_context.hpp>

namespace Marvin {

class RequestHandlerInterface;

typedef std::shared_ptr<RequestHandlerInterface> RequestHandlerInterfaceSPtr;
typedef std::unique_ptr<RequestHandlerInterface> RequestHandlerInterfaceUPtr;

typedef std::function<RequestHandlerInterfaceSPtr(boost::asio::io_service& io)> RequestHandlerSPtrFactory;
typedef std::function<RequestHandlerInterfaceUPtr(boost::asio::io_service& io)> RequestHandlerUPtrFactory;
typedef std::function<void()> HandlerDoneCallbackType;

typedef std::function<RequestHandlerInterface*(boost::asio::io_service& io)> RequestHandlerFactory;

class RequestHandlerInterface
{
public:
    // RequestHandlerInterface(boost::asio::io_service& io);
    
    // virtual ~RequestHandlerInterface();
    
    virtual void handle(
        ServerContext&            server_context,
        ISocketSPtr               clientConnectionSPtr,
        HandlerDoneCallbackType   done
    ) = 0;
    virtual ~RequestHandlerInterface()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

} // namespace Marvin

#endif // HTTP_REQUEST_HANDLER_INTERFACE_HPP
