#include <marvin/collector/pipe_collector.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <regex>
#include <marvin/boost_stuff.hpp>
#include <regex>
#include <marvin/error_handler/error_handler.hpp>
#include <trog/trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

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
    if( msg.hasHeader(Marvin::HeadersV2::ContentType) ){
        hv = msg.getHeader(Marvin::HeadersV2::ContentType);
        capture = headerValueMatched(hv, regexs);
    }
    return capture;
}

static bool testPipeReaderExists(char* pipeName)
{

    int fdw = open(pipeName, O_WRONLY | O_NONBLOCK);
    if (fdw == -1){
        TROG_WARN("non-blocking open for write with no readers failed");
        MARVIN_THROW("non-blocking open for write with no readers failed");
        return false;
    }
    close(fdw);
    return true;
}

std::atomic<PipeCollector*> PipeCollector::s_atomic_instance{nullptr};
std::mutex PipeCollector::s_mutex;
std::string PipeCollector::s_pipe_path = "";


PipeCollector::PipeCollector(boost::asio::io_service& io): m_io(io), m_my_strand(io)
{
   TROG_TRACE_CTOR();
    std::string& tmpPath = s_pipe_path;
    int fdw = open(s_pipe_path.c_str(), O_WRONLY | O_NONBLOCK);
    
    char* n = (char*)s_pipe_path.c_str();
    
    if( testPipeReaderExists( (char*)s_pipe_path.c_str()) ){
        m_out_pipe.open(s_pipe_path, std::ios_base::out);
        m_pipe_open = true;
    }else{
        m_pipe_open = false;
    }
}
/// \see http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/
//PipeCollector* PipeCollector::getInstance(boost::asio::io_service& io)
PipeCollector* PipeCollector::getInstance(boost::asio::io_service& io)
{
    PipeCollector* tmp = s_atomic_instance.load();
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(s_mutex);
        tmp = s_atomic_instance.load();
        if (tmp == nullptr) {
            tmp = new PipeCollector(io);
            s_atomic_instance.store(tmp);
        }
    }
    return tmp;
}
void PipeCollector::configSet_PipePath(std::string path)
{
    s_pipe_path = path;
}
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void PipeCollector::postedCollect(
    std::string scheme,
    std::string host,
    MessageReaderSPtr req,
    MessageBaseSPtr resp)
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
        temp << (req->getContentBuffer())->to_string();
    }
    temp << std::endl;
    temp << "RESPONSE : ========" << std::endl;
    temp << "HTTP/" << resp->httpVersMajor() << "." << resp->httpVersMinor() << " ";
    temp << resp->statusCode() << " " << resp->status() << std::endl;
    auto respHeaders = resp->getHeaders();
    resp->dumpHeaders(temp);
    if( bodyIsCollectable(*resp, regexs) ){
        if (resp->getContentBuffer() != nullptr) {
            auto s = resp->getContentBuffer()->to_string();
            temp << (resp->getContentBuffer())->to_string() << std::endl;
        } else {
            temp << "[]" << std::endl;
        }
    }

    temp << "------------------------------------------------" << std::endl;
    /**
    ** Now write out that description
    **/
    if(! m_pipe_open )
        return;

    m_out_pipe << temp.str();
    m_out_pipe.flush();
    std::cout << temp.str() << std::endl;
}
/**
** Interface method for client code to call collect
**/
void PipeCollector::collect(
    std::string scheme,
    std::string host,
    MessageReaderSPtr req,
    MessageBaseSPtr resp)
{
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/

    auto pf = m_my_strand.wrap(std::bind(&PipeCollector::postedCollect, this, scheme, host, req, resp));
    m_io.post(pf);
}
} // namespace    

