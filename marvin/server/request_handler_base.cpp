#include "boost_stuff.hpp"
#include "request_handler_base.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

RequestHandlerBase::RequestHandlerBase(boost::asio::io_service& io): m_io(io)
{
    
}

RequestHandlerBase::~RequestHandlerBase()
{
    LogDebug("");
}
