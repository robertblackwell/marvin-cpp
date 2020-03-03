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
#include<marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/client/request.hpp>
#include <marvin/collector/collector_interface.hpp>
//#include <marvin/forwarding/forwarding_handler.hpp>

///
/// An instance is used to funnel a copy of the traffic seen by a mitm proxy to a named pipe where it can
/// be read by whatever program one wants to use to display such traffic.
///
/// This class is a singleton that requires to be primed with the servers io_service object so that
/// it (this class) can schedule callbacks. I chose to have the io_service passed in the getInstance
/// static method, that way I cannot forget to pass it.
///
/// In addition it needs the path name of the pipe to which it will write. That pipe name should be set via the static method
/// configSet_PipePath during the startup phase of the server. 
/// Note the creation of a PipeCollector instance will fail if there pipe does not exist or does not have a reader attached.
///
/// The indirect setting of the pipe name is a bit of a problem and I should probably have made the name an argument to the
/// getInstance() method (or at least had a variant that takes the pipe name)
///
/// This class also has a boost strand to protect against mutiple access to critical sections of code.
/// This class also does synchronous stream IO which will delay whatever thread the strand is running on
/// To prevent this delaying the entire proxy process the server should start an additional thread and connect it
/// to the general io_service to aleviate this issue.
///
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
            MessageReaderSPtr req,
            Marvin::Http::MessageBaseSPtr resp);
    
    private:
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void postedCollect(
            std::string scheme,
            std::string host,
            MessageReaderSPtr req,
            Marvin::Http::MessageBaseSPtr resp);

        boost::asio::io_service::strand     m_my_strand;
        boost::asio::io_service&            m_io;
        std::ofstream                       m_out_pipe;
        bool                                m_pipe_open;
};
#endif



