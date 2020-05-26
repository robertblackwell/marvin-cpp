//
// The main entry point for Marvin - a mitm proxy for http/https 
//

#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

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
#include <marvin/buffer/buffer.hpp>               

namespace Marvin {

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
{
    TROG_TRACE_CTOR ();
}

/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void CollectorBase::posted_collect(
        std::string scheme,
        std::string host,
        MessageReaderSPtr req,
        Marvin::MessageBaseSPtr resp)
{
    
    /**
    ** Here implement the transmission of any data using sync or async IO
    **/
    std::stringstream temp;
    
    temp << "------------------------------------------------" << std::endl;
    temp << "HOST: " << scheme << "://" << host << std::endl;
    temp << "REQUEST : =========" << std::endl;
    temp << req->method_string() << " " << req->target() << " ";
    temp << "HTTP/" << req->version_major() << "." << req->version_minor() << std::endl;
    auto reqHeaders = req->headers();
    req->dump_headers(temp);
    temp << "Body: " << std::endl << (req->get_body_buffer_chain())->to_string();
    temp << std::endl;
    temp << "RESPONSE : ========" << std::endl;
    temp << "HTTP/" << resp->version_major() << "." << resp->version_minor() << " ";
    temp << resp->status_code() << " " << resp->reason() << std::endl;
    auto respHeaders = resp->headers();
    resp->dump_headers(temp);
#if 0
     if (resp->getContentBuffer() != nullptr) {
         auto s = resp->getContentBuffer()->to_string();
         temp << (resp->getContentBuffer())->to_string() << std::endl;
     } else {
         temp << "[]" << std::endl;
     }
#endif
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
        Marvin::MessageBaseSPtr resp)
{
    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/
    auto pf = m_my_strand.wrap(std::bind(&CollectorBase::posted_collect, this, scheme, host, req, resp));
    m_io.post(pf);
}
} // namespace

