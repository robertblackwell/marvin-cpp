//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "rb_logger.hpp"
#include "boost_stuff.hpp"
#include "http_server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handler.hpp"

///
/// This class is a singleton, but it must be given a reference to the servers io_service object
/// so that it can schedule callbacks. Have decided to pass the io_service in the static getInstance
/// method so that this cannot be forgotten
///
class ObjcCollector
{
    public:
        static bool             _firstTime;
        static ObjcCollector*   _instance;
        static void*            _anonDelegate;
    
        static ObjcCollector* getInstance(boost::asio::io_service& io);
        static void setDelegate(void* delegate);
        /**
        ** Delete copy constructors
        **/
        ObjcCollector(ObjcCollector const&)   = delete;
        void operator=(ObjcCollector const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(
                std::string scheme,
                std::string host,
                MessageReaderSPtr req,
                MessageWriterSPtr resp);
    
    private:
        ObjcCollector(boost::asio::io_service& io);
        /**
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void postedCollect(
                std::string& scheme,
                std::string& host,
                MessageReaderSPtr req,
                MessageWriterSPtr resp);

        boost::asio::strand         _myStrand;
        boost::asio::io_service&    _ioLoop;
};




