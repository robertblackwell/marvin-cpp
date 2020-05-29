//
// The main entry point for Marvin - a mitm proxy for http/https 
//
#ifndef marvin_pipe_collector_hpp
#define marvin_pipe_collector_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/configure_trog.hpp>

#include <marvin/collector/collector_interface.hpp>
//#include <marvin/forwarding/forwarding_handler.hpp>

namespace Marvin {

/*
* \ingroup collector
* \brief PipeCollector distributes captured traffic to a named pipe.
*
* An instance is used to funnel a copy of the traffic seen by a mitm proxy to a named pipe where it can
* be read by whatever program one wants to use to display such traffic.
*
* This class needs the path name of the pipe to which it will write.
*/
class PipeCollector : public ICollector
{
    public:
        static std::atomic<PipeCollector*>  s_atomic_instance;
        static std::mutex                   s_mutex;
        static std::string                  s_pipe_path;
    
        static PipeCollector* getInstance(boost::asio::io_service& io);
//        static PipeCollector* getInstance(boost::asio::io_service& io);
        static void configSet_PipePath(std::string path);
        
         PipeCollector(boost::asio::io_service& io);
       /**
        ** Delete copy constructors
        **/
        PipeCollector(PipeCollector const&)   = delete;
        void operator=(PipeCollector const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(
            std::string scheme,
            std::string host,
            MessageBase::SPtr req,
            MessageBase::SPtr resp);
    
    private:
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void posted_collect(
            std::string scheme,
            std::string host,
            MessageBase::SPtr req,
            MessageBase::SPtr resp);

        boost::asio::io_service::strand     m_my_strand;
        boost::asio::io_service&            m_io;
        std::ofstream                       m_out_pipe;
        bool                                m_pipe_open;
};
} // namespace
#endif



