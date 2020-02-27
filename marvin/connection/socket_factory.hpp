#ifndef marvin_socket_factory_hpp
#define marvin_socket_factory_hpp
/**
* \ingroup SocketIO
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/include/callback_typedefs.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;
/**
* \ingroup SocketIO
* \brief Factor function to construct either TCP or SSL socket based on inpput arguments
*/
ISocketSPtr socketFactory(
            bool serverFlag,
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
);
/**
* \ingroup SocketIO
* \brief Factor function to construct either TCP or SSL socket based on inpput arguments
*/
ISocketSPtr socketFactory(
            bool serverFlag,
            boost::asio::io_service& io_service,
            const std::string scheme
);

#endif
