#ifndef marvin_socket_factory_hpp
#define marvin_socket_factory_hpp
/**
* \ingroup SocketIO
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include "boost_stuff.hpp"
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"
#include "i_socket.hpp"
#include "buffer.hpp"
#include "i_socket.hpp"

using namespace boost;
using namespace boost::system;
using namespace boost::asio;


using ip::tcp;
using system::error_code;
/**
* \ingroup SocketIO
* \brief Factor fuunction to construct either TCP or SSL socket based on inpput arguments
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
* \brief Factor fuunction to construct either TCP or SSL socket based on inpput arguments
*/
ISocketSPtr socketFactory(
            bool serverFlag,
            boost::asio::io_service& io_service,
            const std::string scheme
);

#endif
