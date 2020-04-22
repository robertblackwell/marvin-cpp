#ifndef marvin_server_v3_timer_hpp
#define marvin_server_v3_timer_hpp

#include <string>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <trog/trog.hpp>
namespace Marvin {
class ATimer;
typedef std::shared_ptr<ATimer> ATimerSPtr;
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
}
#endif /* test_server_h */
