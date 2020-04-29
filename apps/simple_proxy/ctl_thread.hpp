
#ifndef marvin_mitm_xctl_thread_hpp
#define marvin_mitm_xctl_thread_hpp

#include <string>
#include <thread>
#include <boost/optional.hpp>
#include <boost/process/async_pipe.hpp>
#include "timer.hpp"

namespace Marvin {

class CtlProcessor
{
    public:
    CtlProcessor(std::string ctl_pipe_path): m_io(1), m_heartbeat_timer(m_io), m_pipe(m_io, ctl_pipe_path)
    {
        m_heartbeat_interval_ms = 1000;
        m_terminate_requested = false;
        std::string ctl_pipe = "/home/robert/Projects/marvin++/.marvin/marvin_ctl";

    }
    void start()
    {
        try_read();
        p_start_heartbeat();
        m_io.run();

    }
    void try_read()
    {
        // char buf[1000];
        // boost::asio::async_read(m_pipe, boost::asio::buffer(buf),  [](const boost::system::error_code &ec, std::size_t size) 
        // {
        //     std::cout << "read got something" << std::endl;
        // });
            
    }
    void p_on_heartbeat(const boost::system::error_code& ec)
    {
        std::cout << "p_on_heartbeat" << std::endl;
        if(m_terminate_requested) {
            std::cout << "p_on_heartbeat - exiting" << std::endl;
            return;
        }
        p_start_heartbeat();
    }
    void p_start_heartbeat()
    {
        m_heartbeat_timer.expires_from_now(boost::posix_time::milliseconds(m_heartbeat_interval_ms));
        auto ds = (boost::bind(&CtlProcessor::p_on_heartbeat, this, boost::asio::placeholders::error));
        m_heartbeat_timer.async_wait(ds);
    }
    boost::asio::io_service     m_io;
    boost::process::async_pipe  m_pipe;
    long                        m_heartbeat_interval_ms;
    bool                        m_terminate_requested;
    boost::asio::deadline_timer m_heartbeat_timer;
};

class CtlThread
{
    public:
    CtlThread(std::string ctl_pipe_path): m_ctl_processor(ctl_pipe_path)
    {
        m_thread_uptr = std::make_unique<std::thread>([this]()
        {
            m_ctl_processor.start();
            std::cout << "after ctl_processor exit" << std::endl;
        });

    }

    ~CtlThread()
    {

    }
    void post(std::function<void()> f) {

    }

    void join() {
        m_thread_uptr->join();
    }

    void terminate() {

    }
    CtlProcessor                 m_ctl_processor;
    bool                         m_cancel_flag;
    std::unique_ptr<std::thread> m_thread_uptr;    
};
}

#endif
