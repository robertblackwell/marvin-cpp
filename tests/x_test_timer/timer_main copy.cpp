//
// Aim of this experiment is to demonstrate and then fix the problem of a handler being called after the timer
// is destroyed
//
#include <iostream>
#include <marvin/boost_stuff.hpp>
#define USE_STRANDx
class ATimer
{
public:
    ATimer(boost::asio::io_service& io, std::string my_id)
    :m_io(io), m_count(0), m_id(my_id)
    {
    }
    void arm(long interval_secs, std::function<void()> cb)
    {
        m_callback = cb;
        m_timer1_sptr = std::make_shared<boost::asio::deadline_timer>(m_io, boost::posix_time::seconds(interval_secs));
        m_timer1_sptr->async_wait(std::bind(&ATimer::handler, this, std::placeholders::_1));
    }
    
    ~ATimer()
    {
        std::cout << "destructor " << m_id << std::endl;
        m_id = "deleted";
        m_callback = nullptr;
    }
    
    void handler(const boost::system::error_code& err)
    {
        std::cout << "Timer handler id:" << m_id << std::endl;
        m_callback();
    }
    

    boost::asio::io_service& m_io;
    std::string m_id;
    std::shared_ptr<boost::asio::deadline_timer> m_timer1_sptr;
    int m_count;
    std::function<void()> m_callback;
};

int main()
{
    boost::asio::io_service io;
    std::shared_ptr<ATimer> t1_sptr = std::make_shared<ATimer>(io, "TIMER1");
    std::shared_ptr<ATimer> t2_sptr = std::make_shared<ATimer>(io, "TIMER2");
    t1_sptr->arm(1, [&io, &t1_sptr, &t2_sptr](){
        t2_sptr->m_timer1_sptr->cancel();
        t2_sptr = std::make_shared<ATimer>(io, "TIMER1");
        std::cout << "callback 1" << std::endl;
    });
    t2_sptr->arm(10, [&t1_sptr, &t2_sptr](){
        std::cout << "callback 2" << std::endl;
    });
    io.run();
    
    return 0;
}
