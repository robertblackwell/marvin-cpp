//
// The main entry point for Marvin - a mitm proxy for http/https 
//

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include <regex>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include "pipe_collector.hpp"

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
    if( msg.hasHeader(HttpHeader::Name::ContentType) ){
        hv = msg.getHeader(HttpHeader::Name::ContentType);
        capture = headerValueMatched(hv, regexs);
    }
    return capture;
}

static bool testPipeReaderExists(char* pipeName)
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
        static PipeCollector*   _instance;

PipeCollector::PipeCollector(boost::asio::io_service& io): _ioLoop(io), _myStrand(io)
{
    LogTorTrace();
    std::string& tmpPath = _pipePath;
    int fdw = open(_pipePath.c_str(), O_WRONLY | O_NONBLOCK);
    
    char* n = (char*)_pipePath.c_str();
    
    if( testPipeReaderExists( (char*)_pipePath.c_str()) ){
        _outPipe.open("/Users/rob/marvin_collect", std::ios_base::out);
        _pipeOpen = true;
    }else{
        _pipeOpen = false;
    }
}
    
PipeCollector* PipeCollector::getInstance(boost::asio::io_service& io)
{
    if( _firstTime ){
        _firstTime = false;
        _instance = new PipeCollector(io);
    }
    return _instance;
}
void PipeCollector::configSet_PipePath(std::string path)
{
    _pipePath = path;
}
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void PipeCollector::postedCollect(
    std::string& scheme,
    std::string& host,
    MessageReaderSPtr req,
    MessageWriterSPtr resp)
{
    
    std::vector<std::regex> regexs;
    regexs.push_back(std::regex("^text\\/(.)*$"));
    regexs.push_back(std::regex("^application\\/(.)*$/"));
    /**
    ** Here implement the transmission of any data using sync or async IO
    **/
    std::stringstream temp;
    
    temp << "------------------------------------------------" << std::endl;
    temp << "HOST: " << scheme << "://" << host << std::endl;
    temp << "REQUEST : =========" << std::endl;
    temp << req->getMethodAsString() << " " << req->uri() << " ";
    temp << "HTTP/" << req->httpVersMajor() << "." << req->httpVersMinor() << std::endl;
    auto reqHeaders = req->getHeaders();
    req->dumpHeaders(temp);
    if( bodyIsCollectable(*req, regexs) ){
        temp << req->getBody();
    }
    temp << std::endl;
    temp << "RESPONSE : ========" << std::endl;
    temp << "HTTP/" << resp->httpVersMajor() << "." << resp->httpVersMinor() << " ";
    temp << resp->statusCode() << " " << resp->status() << std::endl;
    auto respHeaders = resp->getHeaders();
    resp->dumpHeaders(temp);
    if( bodyIsCollectable(*resp, regexs) ){
        temp << resp->getBody() << std::endl;
    }

    temp << "------------------------------------------------" << std::endl;
    /**
    ** Now write out that description
    **/
    if(! _pipeOpen )
        return;

    _outPipe << temp.str();
    _outPipe.flush();
}
/**
** Interface method for client code to call collect
**/
void PipeCollector::collect(
    std::string& scheme,
    std::string& host,
    MessageReaderSPtr req,
    MessageWriterSPtr resp)
{
    std::cout << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;

    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/

    auto pf = _myStrand.wrap(std::bind(&PipeCollector::postedCollect, this, scheme, host, req, resp));
    _ioLoop.post(pf);
}
    
bool PipeCollector::_firstTime = true;
PipeCollector* PipeCollector::_instance = NULL;
std::string PipeCollector::_pipePath = "";


