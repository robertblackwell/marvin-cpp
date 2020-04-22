#include <marvin/boost_stuff.hpp>

#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/connection_handler.hpp>

#include <trog/trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

using namespace Marvin; 

// RequestHandlerInterface::RequestHandlerInterface(boost::asio::io_service& io): m_io(io)
// {

// }

// RequestHandlerInterface::~RequestHandlerInterface()
// {
//     TROG_DEBUG("");
// }