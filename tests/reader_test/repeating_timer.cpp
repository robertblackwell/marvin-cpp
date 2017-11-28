#include "boost_stuff.hpp"
#include <functional>
#include <memory>

#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG);

#include "repeating_timer.hpp"

SingleTimer::SingleTimer(boost::asio::io_service& io, int millSeconds): io_(io), interval_(millSeconds)
{
    t_ = new boost::asio::deadline_timer(io_);
}
SingleTimer::~SingleTimer()
{
    LogDebug("");
    t_->cancel();
    delete t_;
}

void
SingleTimer::start(std::function<void(const boost::system::error_code& ec)> cb){
    callBack = cb;
    t_->expires_from_now(boost::posix_time::milliseconds(interval_));
    auto ds = boost::bind(&SingleTimer::doSomething, this, boost::asio::placeholders::error);
    t_->async_wait(ds);
}

void
SingleTimer::doSomething(const boost::system::error_code& ec){
    callBack(ec);
}

RepeatingTimer::RepeatingTimer(boost::asio::io_service& io, int millSeconds): io_(io), interval_(millSeconds)
{
    t_ = new boost::asio::deadline_timer(io_);
}
RepeatingTimer::~RepeatingTimer()
{
    t_->cancel();
    delete t_;
}

void
RepeatingTimer::start(RepeatingTimerCallBack cb){
    callBack = cb;
    t_->expires_from_now(boost::posix_time::milliseconds(interval_));
    auto ds = boost::bind(&RepeatingTimer::doSomething, this, boost::asio::placeholders::error);
    t_->async_wait(ds);        
}
void
RepeatingTimer::doSomething(const boost::system::error_code& ec){
    
    if( ! callBack(ec) ) return;
    
    t_->expires_from_now(boost::posix_time::milliseconds(interval_));
    auto ds = boost::bind(&RepeatingTimer::doSomething, this, boost::asio::placeholders::error);
    t_->async_wait(ds);
    
}
