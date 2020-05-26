#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/boost_stuff.hpp>

#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/server_v3/connection_handler.hpp>



using namespace Marvin; 

// RequestHandlerInterface::RequestHandlerInterface(boost::asio::io_service& io): m_io(io)
// {

// }

// RequestHandlerInterface::~RequestHandlerInterface()
// {
//     TROG_DEBUG("");
// }