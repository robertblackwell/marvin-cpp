/**
* This file implements the steps required to write and full message whether it does or does not have
* a body. 
*/
#include <marvin/client/request.hpp>
#include <cassert>                                      // for assert
#include <istream>                                      // for string
#include <marvin/connection/socket_factory.hpp>         // for socketFactory
#include <marvin/external_src/rb_logger/rb_logger.hpp>  // for LogInfo, LogD...
#include <marvin/message/message_reader.hpp>            // for MessageReader
#include <memory>                                       // for operator!=
#include <string>                                       // for to_string
#include <boost/asio/streambuf.hpp>                     // for streambuf
#include <cert/error.hpp>                               // for THROW
#include <marvin/http/http_header.hpp>                  // for Headers, Head...
#include <marvin/http/uri.hpp>                          // for Uri
#include <marvin/message/message_writer.hpp>            // for MessageWriter
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }

using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::io_service;
using boost::asio::streambuf;
using namespace Marvin;
using namespace Marvin::Http;

//--------------------------------------------------------------------------------
// come here to do a write of the full message
//--------------------------------------------------------------------------------
void Request::p_check_connected_before_internal_write_message()
{
    if(m_is_connected) {
        p_internal_write_message();
    } else {
        p_internal_connect_before_write_message();
    }
}
void Request::p_internal_connect_before_write_message()
{
    LogInfo("", (long)this);
    using namespace Marvin;
    std::function<void(ErrorType& err)> h = [](ErrorType& err) {

    };
    asyncConnect([this](Marvin::ErrorType& ec){
        LogDebug("cb-connect");
        if(!ec) {
            p_internal_write_message();
        } else {
            m_response_handler(ec, m_rdr);
        }
    });
}
void Request::p_internal_write_message()
{
    LogInfo("", (long)this);

    // we are about to write the entire request message
    // so make sure we have the content-length correct
    p_set_content_length();
    LogInfo("",traceWriter(*m_wrtr));
    
    assert(m_body_mbuffer_sptr != nullptr);
    auto req_str = m_current_request->str();
    m_wrtr->asyncWrite(m_current_request, m_body_mbuffer_sptr, [this](Marvin::ErrorType& ec){
        if (!ec) {
            LogDebug("start read");
            this->m_rdr->readMessage([this](Marvin::ErrorType ec2){
                auto resp_str = m_rdr->str();
                auto bdy_str = m_rdr->getContent()->to_string();
                if (!ec2) {
                    this->m_response_handler(ec2, m_rdr);
                } else {
                    this->m_response_handler(ec2, m_rdr);
                }
            });
        } else {
            this->m_response_handler(ec, m_rdr);
        }
    });
}
