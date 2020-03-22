//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <marvin/collector/collector_base.hpp>
#include <stdio.h>                                      // for perror
#include <sys/fcntl.h>                                  // for open, O_NONBLOCK
#include <unistd.h>                                     // for close
#include <functional>                                   // for __bind, bind
#include <iostream>                                     // for operator<<
#include <memory>                                       // for shared_ptr
#include <new>                                          // for operator new
#include <string>                                       // for basic_string
#include <boost/asio/detail/impl/strand_service.hpp>    // for strand_servic...
#include <boost/asio/impl/io_context.hpp>               // for io_context::post
#include <marvin/buffer/buffer_chain.hpp>               // for BufferChain
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

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
CollectorBase::CollectorBase(boost::asio::io_service& io, std::ostream& ofstream): m_io(io), m_my_strand(io), m_output_stream(ofstream)
{}

#if 0    
CollectorBase::CollectorBase(boost::asio::io_service& io, boost::filesystem::path file_path): m_io(io), m_my_strand(io)
{
    if( testPipeReaderExists((char*)"/Users/robertblackwell/marvin_collector") ){
        m_output_stream.open(file_path.string(), std::ios_base::out);
        if(!m_output_stream.is_open()) {
            throw "failed to open stream for collector";
        }
    }
}
#endif
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
    std::stringstream temp;
    
    temp << "------------------------------------------------" << std::endl;
    temp << "HOST: " << scheme << "://" << host << std::endl;
    temp << "REQUEST : =========" << std::endl;
    temp << req->getMethodAsString() << " " << req->uri() << " ";
    temp << "HTTP/" << req->httpVersMajor() << "." << req->httpVersMinor() << std::endl;
    auto reqHeaders = req->getHeaders();
    req->dumpHeaders(temp);
    temp << "Body: " << std::endl << (req->getContentBuffer())->to_string();
    temp << std::endl;
    temp << "RESPONSE : ========" << std::endl;
    temp << "HTTP/" << resp->httpVersMajor() << "." << resp->httpVersMinor() << " ";
    temp << resp->statusCode() << " " << resp->status() << std::endl;
    auto respHeaders = resp->getHeaders();
    resp->dumpHeaders(temp);
    if (resp->getContentBuffer() != nullptr) {
        auto s = resp->getContentBuffer()->to_string();
        temp << (resp->getContentBuffer())->to_string() << std::endl;
    } else {
        temp << "[]" << std::endl;
    }

    temp << "------------------------------------------------" << std::endl;
    m_output_stream << temp.str();
    m_output_stream.flush();
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
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/
    auto pf = m_my_strand.wrap(std::bind(&CollectorBase::postedCollect, this, scheme, host, req, resp));
    m_io.post(pf);
}


