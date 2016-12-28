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

#include "pipe_collector.hpp"

bool testPipeReaderExists(char* pipeName)
{

    int fdw = open(pipeName, O_WRONLY | O_NONBLOCK);
    if (fdw == -1){
        perror("non-blocking open for write with no readers failed");
        return false;
    }
    close(fdw);
    return true;
}

        static bool             _firstTime;
        static NullCollector*   _instance;

PipeCollector::PipeCollector(boost::asio::io_service& io): _ioLoop(io), _myStrand(io)
{
    if( testPipeReaderExists((char*)"/Users/rob/marvin_collect") ){
        _outPipe.open("/Users/rob/marvin_collect", std::ios_base::out);
        _pipeOpen = true;
    }else{
        _pipeOpen = false;
    }
}
    
static PipeCollector::PipeCollector* getInstance(boost::asio::io_service& io)
{
    if( _firstTime ){
        _firstTime = false;
        _instance = new NullCollector(io);
    }
    return _instance;
}
    
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void PipeCollector::postedCollect(MessageReaderSPtr req, MessageWriterSPtr resp)
{
    
    /**
    ** Here implement the transmission of any data using sync or async IO
    **/
    _outPipe << "ZXCVBNNM<HJKJLK" << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;
}
/**
** Interface method for client code to call collect
**/
void PipeCollector::collect(MessageReaderSPtr req, MessageWriterSPtr resp)
{
    std::cout << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;
    if(! _pipeOpen )
        return;
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/

    if(! _pipeOpen )
        return;
    auto pf = _myStrand.wrap(std::bind(&NullCollector::postedCollect, this, req, resp));
    _ioLoop.post(pf);
}
    
PipeCollector::PipeCollector(boost::asio::io_service& io): _ioLoop(io), _myStrand(io)
{
    if( testPipeReaderExists((char*)"/Users/rob/marvin_collect") ){
        _outPipe.open("/Users/rob/marvin_collect", std::ios_base::out);
        _pipeOpen = true;
    }else{
        _pipeOpen = false;
    }
}

bool NullCollector::_firstTime = true;
NullCollector* NullCollector::_instance = NULL;


