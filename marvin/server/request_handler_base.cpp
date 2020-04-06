#include <marvin/boost_stuff.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
namespace Marvin {
RequestHandlerBase::RequestHandlerBase(boost::asio::io_service& io): m_io(io)
{

}

RequestHandlerBase::~RequestHandlerBase()
{
    LogDebug("");
}
}