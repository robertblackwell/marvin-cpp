#include <marvin/boost_stuff.hpp>

#include <marvin/server_v2/server_connection_manager_v2.hpp>
#include <marvin/server_v2/request_handler_base_v2.hpp>
#include <marvin/server_v2/connection_handler_v2.hpp>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

using namespace Marvin; 

RequestHandlerBaseV2::RequestHandlerBaseV2(boost::asio::io_service& io): m_io(io)
{

}

RequestHandlerBaseV2::~RequestHandlerBaseV2()
{
    LogDebug("");
}