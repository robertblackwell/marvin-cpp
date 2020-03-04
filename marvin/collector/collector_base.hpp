//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include<marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/collector/collector_interface.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>

class CollectorBase: public ICollector
{
    public:
        CollectorBase(boost::asio::io_service& io);
        // CollectorBase(boost::asio::io_service& io, boost::filesystem::path file_path);
        CollectorBase(boost::asio::io_service& io, std::ostream& out_stream);

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
        boost::asio::io_service&                m_io;
        std::ostream&                           m_output_stream;
    
};


