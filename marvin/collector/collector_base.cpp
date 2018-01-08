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
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "collector_base.hpp"

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

bool             CollectorBase::s_first_time;
CollectorBase*   CollectorBase::s_instance;
    
CollectorBase* CollectorBase::getInstance(boost::asio::io_service& io)
{
    if( s_first_time ){
        s_first_time = false;
        s_instance = new CollectorBase(io);
    }
    return s_instance;
}
    
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void CollectorBase::postedCollect(
        std::string scheme,
        std::string host,
        MessageReaderSPtr req,
        Marvin::Http::MessageBaseSPtr resp)
{
    
    /**
    ** Here implement the transmission of any data using sync or async IO
    **/
    m_out_pipe << "ZXCVBNNM<HJKJLK" << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;
}
/**
** Interface method for client code to call collect
**/
void CollectorBase::collect(
        std::string scheme,
        std::string host,
        MessageReaderSPtr req,
        Marvin::Http::MessageBaseSPtr resp)
{
    std::cout << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;
    if(! m_pipe_open )
        return;
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/
    auto pf = m_my_strand.wrap(std::bind(&CollectorBase::postedCollect, this, scheme, host, req, resp));
    m_io.post(pf);
}
    
CollectorBase::CollectorBase(boost::asio::io_service& io): m_io(io), m_my_strand(io)
{
    if( testPipeReaderExists((char*)"/Users/rob/marvin_collect") ){
        m_out_pipe.open("/Users/rob/marvin_collect", std::ios_base::out);
        m_pipe_open = true;
    }else{
        m_pipe_open = false;
    }
}


