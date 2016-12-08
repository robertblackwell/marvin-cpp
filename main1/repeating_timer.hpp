#ifndef repeatingtimer_h
#define repeatingtimer_h

typedef std::function<void(const boost::system::error_code&)> SingleTimerCallBack;
typedef std::function<bool(const boost::system::error_code&)> RepeatingTimerCallBack;

class SingleTimer
{
public:
    SingleTimer(boost::asio::io_service& io, int millSeconds): io_(io), interval_(millSeconds)
    {
        t_ = new boost::asio::deadline_timer(io_);
    }
    void start(std::function<void(const boost::system::error_code& ec)> cb){
        callBack = cb;
        t_->expires_from_now(boost::posix_time::milliseconds(interval_));
        auto ds = boost::bind(&SingleTimer::doSomething, this, boost::asio::placeholders::error);
        t_->async_wait(ds);
    }
private:
    void doSomething(const boost::system::error_code& ec){
        callBack(ec);
    }
    int interval_;
    boost::asio::io_service& io_;
    SingleTimerCallBack callBack;
    boost::asio::deadline_timer* t_;
};

class RepeatingTimer
{
public:
    RepeatingTimer(boost::asio::io_service& io, int millSeconds): io_(io), interval_(millSeconds)
    {
        t_ = new boost::asio::deadline_timer(io_);
    }
    void start(RepeatingTimerCallBack cb){
        callBack = cb;
        t_->expires_from_now(boost::posix_time::milliseconds(interval_));
        auto ds = boost::bind(&RepeatingTimer::doSomething, this, boost::asio::placeholders::error);
        t_->async_wait(ds);        
    }
private:
    void doSomething(const boost::system::error_code& ec){
        
        if( ! callBack(ec) ) return;
        
        t_->expires_from_now(boost::posix_time::milliseconds(interval_));
        auto ds = boost::bind(&RepeatingTimer::doSomething, this, boost::asio::placeholders::error);
        t_->async_wait(ds);
        
    }
    int interval_;
    boost::asio::io_service& io_;
    RepeatingTimerCallBack callBack;
    boost::asio::deadline_timer* t_;
};

#endif