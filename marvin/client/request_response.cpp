//
// This file implements the fuucntion sequence that reads a response
// an calls the varios event handlers
//
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
#include <marvin/http/header_fields.hpp>                  // for Headers, Head...
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
// start sequence of functions to read response
//--------------------------------------------------------------------------------
void Request::p_read_response_headers()
{
    this->m_rdr->asyn_read_headers([this](Marvin::ErrorType ec2)
                                   {
                                       // auto resp_str = m_rdr->str();
                                       auto bdy_str = m_rdr->get_content()->to_string();
                                       if (!ec2) {
                                           // call onHeaders
                                           // setup read of body unless content length == 0
                                           if (auto hopt = this->m_rdr->header(Marvin::HeadersV2::ContentLength)) {
                                               std::string clstr = hopt.get();
                                               if (clstr != "0") {
                                                   p_read_response_body();
                                               } else {
                                                   p_response_complete();
                                               }
                                           }
                                       } else {
                                           // call on error handler
                                           TROG_ERROR("error on read headers");
                                           p_response_error(ec2);
                                       }
                                   });

}
void Request::p_read_response_body()
{
    this->m_rdr->async_read_body([this](Marvin::ErrorType err, Marvin::BufferChain::SPtr buf_sptr)
                                 {
                                     auto ex = err.value();
                                     if (err) {
                                         p_response_error(err);
                                     } else {
                                         p_read_response_handle_buffer(buf_sptr);
                                         if (err == Marvin::make_error_eob() || err == Marvin::make_error_eom()) {
                                             //call message complete cb
                                             p_response_complete();
                                         } else {
                                             this->p_read_response_body_next();
                                         }
                                     }
                                 });
}
void Request::p_read_response_body_next()
{

}
void Request::p_read_response_handle_buffer(Marvin::BufferChain::SPtr buf_sptr)
{

}
void Request::p_response_complete()
{

}
void Request::p_response_error(Marvin::ErrorType err)
{

}
} // namespace