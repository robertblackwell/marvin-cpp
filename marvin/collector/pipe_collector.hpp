//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"

#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handler.hpp"

///
/// This class is a singleton that requires to be primed with the servers io_service object so that
/// it (this class) can schedule callbacks. I chose to have the io_service passed in the getInstance
/// static method, that way cannot forget
///
/// In addition it needs the path name of the pipe to which it will write. That should be set via the static method
/// configSet_PipePath during the startup phase of the server.
///
/// This class also has a boost strand to protect against mutiple access to critical sections of code.
/// BUT, this class also does synchronous stream IO which will delay whatever thread the strand is running on
/// to prevent this delaying the entire proxy process the server should start an additional thread and connect it
/// to the general io_service to aleviate this issue.
///
class PipeCollector
{
    public:
        static bool             s_first_time;
        static PipeCollector*   s_instance;
        static std::string      s_pipe_path;
    
        static PipeCollector* getInstance(boost::asio::io_service& io);
        static void configSet_PipePath(std::string path);
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
            MessageBaseSPtr resp);
    
    private:
        PipeCollector(boost::asio::io_service& io);
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void postedCollect(
            std::string scheme,
            std::string host,
            MessageReaderSPtr req,
            MessageBaseSPtr resp);

        boost::asio::strand         m_my_strand;
        boost::asio::io_service&    m_io;
        std::ofstream               m_out_pipe;
        bool                        m_pipe_open;
};




