
#ifndef socket_factory_hpp
#define socket_factory_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
//#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
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

ISocket* socketFactory(
            boost::asio::io_service& io_service,
            const std::string scheme,
            const std::string server,
            const std::string port
);
ISocket* socketFactory(
            boost::asio::io_service& io_service,
            const std::string scheme
);

#endif
