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

class CollectorBase
{
    public:
        static bool             _firstTime;
        static CollectorBase*   _instance;
    
        static CollectorBase* getInstance(boost::asio::io_service& io);
        /**
        ** Delete copy constructors
        **/
        CollectorBase(CollectorBase const&)   = delete;
        void operator=(CollectorBase const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(std::string scheme, std::string host, MessageReaderSPtr req, Marvin::Http::MessageBaseSPtr resp);
    
    private:
        CollectorBase(boost::asio::io_service& io);
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

        boost::asio::io_context::strand         m_my_strand;
        boost::asio::io_service&    m_io;
        std::ofstream               m_out_pipe;
        bool                        m_pipe_open;
    
        static bool                 s_first_time;
        static CollectorBase*       s_instance;
};


