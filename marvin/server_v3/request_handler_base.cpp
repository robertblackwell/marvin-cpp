#include <marvin/boost_stuff.hpp>

#include <marvin/server_v3/server_connection_manager.hpp>
#include <marvin/server_v3/request_handler_base.hpp>
#include <marvin/server_v3/connection_handler.hpp>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

using namespace Marvin; 

RequestHandlerBase::RequestHandlerBase(boost::asio::io_service& io): m_io(io)
{

}

RequestHandlerBase::~RequestHandlerBase()
{
    LogDebug("");
}