#ifndef marvin_connecttion_socket_factory_hpp
#define marvin_connection_socket_factory_hpp
#include <string>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/error/marvin_error.hpp>
namespace Marvin {
using namespace ::boost;
using namespace ::boost::system;
using namespace ::boost::asio;
using ip::tcp;
using system::error_code;
/**
* \ingroup connection
*
* \brief Factory function to construct a new TCP Connection and prime it with a server name and port.
* The TCP Connection class can be any combination of  HTTP/HTTPS or Server/Client the distinction is made
* after creation.
* Currently the factory function produces http connections only and the user is responsible
* for priming the connection for https operation. The is once I decide how to provide certificate store
* or server identity capability.
*/
ISocketSPtr socketFactory(
            ::boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
);

ISocketSPtr socketFactory(boost::asio::io_service& io_service);
} // namespace
#endif
