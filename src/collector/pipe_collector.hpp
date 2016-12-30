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

class PipeCollector
{
    public:
        static bool             _firstTime;
        static PipeCollector*   _instance;
        static std::string      _pipePath;
    
        static PipeCollector* getInstance(boost::asio::io_service& io);
        static void setPipePath(std::string path);
        /**
        ** Delete copy constructors
        **/
        PipeCollector(PipeCollector const&)   = delete;
        void operator=(PipeCollector const&)  = delete;
    

        /**
        ** Interface method for client code to call collect
        **/
        void collect(
            std::string& scheme,
            std::string& host,
            MessageReaderSPtr req,
            MessageWriterSPtr resp);
    
    private:
        PipeCollector(boost::asio::io_service& io);
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
        std::ofstream               _outPipe;
        bool                        _pipeOpen;
};




