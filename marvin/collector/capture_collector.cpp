#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/collector/capture_collector.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <regex>
#include <marvin/boost_stuff.hpp>
#include <regex>
#include <marvin/error_handler/error_handler.hpp>

namespace Marvin {

bool headerValueMatched(std::string& hv, std::vector<std::regex>& regexs)
{
    bool capture = false;
    for( std::regex& re : regexs){
        auto res = std::regex_search(hv, re);
        if( res ) {
            capture = true;
            break;
        }
    }
    return capture;
}

bool bodyIsCollectable(MessageBase& msg, std::vector<std::regex>& regexs)
{
    bool capture = false;
    std::string hv;
    if( auto hopt = msg.header(Marvin::HeaderFields::ContentType) ){
        hv = hopt.get();
        capture = headerValueMatched(hv, regexs);
    }
    return capture;
}


CaptureCollector::CaptureCollector(boost::asio::io_service& io, CaptureFilter::SPtr filter_sptr): m_io(io)
{
   TROG_TRACE_CTOR();
}
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void CaptureCollector::posted_collect(
    std::string scheme,
    std::string host,
    MessageReaderSPtr req,
    MessageBaseSPtr resp)
{

}
/**
** Interface method for client code to call collect
**/
void CaptureCollector::collect(
    std::string scheme,
    std::string host,
    MessageReaderSPtr req,
    MessageBaseSPtr resp)
{
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/


}
} // namespace    

