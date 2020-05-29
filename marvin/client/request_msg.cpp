/**
* This file implements the steps required to write and full message whether it does or does not have
* a body. 
*/
#include <marvin/client/request.hpp>
#include <cassert>                                      // for assert
#include <istream>                                      // for string
#include <marvin/connection/socket_factory.hpp>         // for socket_factory
#include <marvin/configure_trog.hpp>  // for LogInfo, LogD...
#include <marvin/message/message_reader_v2.hpp>            // for MessageReader
#include <memory>                                       // for operator!=
#include <string>                                       // for to_string
#include <boost/asio/streambuf.hpp>                     // for streambuf
#include <cert/error.hpp>                               // for MARVIN_THROW
#include <marvin/http/headers_v2.hpp>                  // for Headers, Head...
#include <marvin/http/uri.hpp>                          // for Uri
#include <marvin/message/message_writer.hpp>            // for MessageWriter


namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }

namespace Marvin {
using ::boost::asio::ip::tcp;
using ::boost::system::error_code;
using ::boost::asio::io_service;
using ::boost::asio::streambuf;

//--------------------------------------------------------------------------------
// come here to do a write of the full message
//--------------------------------------------------------------------------------
void Request::p_msg_check_connected(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb)
{
    if(m_is_connected) {
        p_msg_write(msg, mbuf_sptr, cb);
    } else {
        p_msg_connect(msg, mbuf_sptr, cb);
    }
}
void Request::p_msg_connect(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb)
{
    TROG_INFO("", (long)this);
    using namespace Marvin;
    std::function<void(ErrorType& err)> h = [](ErrorType& err) {

    };
    asyncConnect([this, msg, mbuf_sptr, cb](Marvin::ErrorType& ec){
        TROG_DEBUG("cb-connect");
        if(!ec) {
            p_msg_write(msg, mbuf_sptr, cb);
        } else {
            cb(ec);
        }
    });
}
void Request::p_msg_write(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb)
{
    TROG_INFO("", (long)this);

    // we are about to write the entire request message
    // so make sure we have the content-length correct
    p_set_content_length();
    assert(m_body_mbuffer_sptr != nullptr);
    // auto req_str = msg->str();
    m_wrtr->async_write(msg, mbuf_sptr, [this, cb](Marvin::ErrorType& ec){
        if (!ec) {
            TROG_DEBUG("start read");
            this->m_rdr->async_read_message([this](Marvin::ErrorType ec2)
                                            {
                                                // auto resp_str = m_rdr->str();
                                                auto bdy_str = m_rdr->get_content()->to_string();
                                                if (!!ec2) {
                                                    p_resp_on_error(ec2);
                                                } else {
                                                    p_resp_on_headers(ec2, m_rdr);
                                                    auto ec_eom = Marvin::Error::make_eom();
                                                    p_resp_on_data(ec_eom, m_rdr->get_body_buffer_chain());
                                                    p_resp_on_complete(ec2, m_rdr);
                                                }
                                            });
        } else {
            cb(ec);
        }
    });
}
} // namespace