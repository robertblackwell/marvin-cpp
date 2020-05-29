#ifndef marvin_proxy_app_proxy_timer_hpp
#define marvin_proxy_app_proxy_timer_hpp
#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>


class MyTimer
{
public:
   MyTimer(boost::asio::io_service& io_service, int interval_secs)
      : m_timer(io_service), m_interval(interval_secs)
    {
        std::cout << "MyTimer::MyTimer()" << std::endl;;
        m_cancel_flag = false;
   }
   
   ~MyTimer()
   {
      std::cout << "MyTimer::~MyTimer()" << std::endl;
   }

   void arm(std::function<void()> timer_callback)
    {
        m_timer_callback = timer_callback;
        setup_next();
    }
    void terminate() {
        m_cancel_flag = true;
        m_timer.cancel();
    }
    void setup_next()
    {
        std::cout << "MyTimer::arm()" << std::endl;

        m_timer.expires_from_now(std::chrono::seconds(m_interval));
        m_timer.async_wait([this](const boost::system::error_code& ec)
        {
            std::cout << "running lambda 2" << std::endl;

            if (ec || m_cancel_flag) {
               std::cout << "timer ec: " << ec.message() << std::endl;
               return;
            }
            if (!ec && ( ! m_cancel_flag)) {
                setup_next(); // re-arm the timer
                m_timer_callback();;
            }

         });
   }
    
private:
    std::function<void()>       m_timer_callback;
    boost::asio::steady_timer   m_timer;
    int                         m_interval;
    bool m_cancel_flag;
};
#endif