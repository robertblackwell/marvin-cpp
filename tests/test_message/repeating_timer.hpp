#ifndef repeatingtimer_h
#define repeatingtimer_h

typedef std::function<void(const boost::system::error_code&)> SingleTimerCallBack;
typedef std::function<bool(const boost::system::error_code&)> RepeatingTimerCallBack;

class SingleTimer
{
public:
    SingleTimer(boost::asio::io_service& io, int millSeconds);
    ~SingleTimer();
    void start(std::function<void(const boost::system::error_code& ec)> cb);
private:
    void doSomething(const boost::system::error_code& ec);
    
    int interval_;
    boost::asio::io_service& io_;
    SingleTimerCallBack callBack;
    boost::asio::deadline_timer* t_;
};

class RepeatingTimer
{
public:
    RepeatingTimer(boost::asio::io_service& io, int millSeconds);
    ~RepeatingTimer();
    void start(RepeatingTimerCallBack cb);
private:
    void doSomething(const boost::system::error_code& ec);
    int interval_;
    boost::asio::io_service& io_;
    RepeatingTimerCallBack callBack;
    boost::asio::deadline_timer* t_;
};

#endif
