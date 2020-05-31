//
// The main entry point for Marvin - a mitm proxy for http/https 
//

#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/collector/cout_collector.hpp>

#include <iostream>                                     // for operator<<
#include <string>                                       // for basic_string

namespace Marvin {

CoutCollector::CoutCollector(boost::asio::io_service& io, CaptureFilter::SPtr filter_sptr)
: m_io(io), m_filter_sptr(filter_sptr)
{
    TROG_TRACE_CTOR ();
}

/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void CoutCollector::posted_collect(
        std::string scheme,
        std::string host,
        MessageBase::SPtr req,
        MessageBase::SPtr resp)
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
    if (m_filter_sptr->get_show_message_bodies()) {
        if (resp->get_body_buffer_chain() != nullptr) {
            auto s = resp->get_body_buffer_chain()->to_string();
            temp << s << std::endl;
        } else {
            temp << "[]" << std::endl;
        }
    }
    temp << "------------------------------------------------" << std::endl;
    std::cout << temp.str();
    std::cout << std::flush;
}
/**
** Interface method for client code to call collect
**/
void CoutCollector::collect(
        std::string scheme,
        std::string host,
        MessageBase::SPtr req,
        MessageBase::SPtr resp)
{
    m_io.post([this, scheme, host, req, resp]()
    {
        posted_collect(scheme, host, req, resp);
    });
}
} // namespace

