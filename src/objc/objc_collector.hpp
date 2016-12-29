//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handler.hpp"

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
        ** This method actually implements the collect function but run on a dedicated
        ** strand. Even if this method does IO-wait operations the other thread will
        ** keep going
        **/
        void postedCollect(MessageReaderSPtr req, MessageWriterSPtr resp);

        /**
        ** Interface method for client code to call collect
        **/
        void collect(MessageReaderSPtr req, MessageWriterSPtr resp);
    
    private:
        ObjcCollector(boost::asio::io_service& io);

        boost::asio::strand         _myStrand;
        boost::asio::io_service&    _ioLoop;
};




