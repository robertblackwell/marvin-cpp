//
// Implements the steps to write headers only
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
// async_write_headers
//
//--------------------------------------------------------------------------------
void Request::asyncWriteHeaders(WriteHeadersCallbackType cb)
{
    p_check_connected_before_write_headers(cb);
    // return;
    // p_assert_not_headers_written();
    // p_add_chunked_encoding_header();
    // p_prep_write_complete_headers();
    // p_check_connected_before_write_headers(cb);
    // m_wrtr->async_write_headers(m_current_request, [this, cb](Marvin::ErrorType err){
    //     m_headers_written = true;
    //     cb(err);
    // });
}
void Request::p_check_connected_before_write_headers(WriteHeadersCallbackType write_headers_cb)
{
    if(m_is_connected) {
        p_internal_write_headers(write_headers_cb);
    } else {
        p_internal_connect_before_write_headers(write_headers_cb);
    }
}
void Request::p_internal_connect_before_write_headers(WriteHeadersCallbackType write_headers_cb)
{
    TROG_INFO("", (long)this);
    using namespace Marvin;
    std::function<void(ErrorType& err)> h = [](ErrorType& err) {

    };
    asyncConnect([this, write_headers_cb](Marvin::ErrorType& ec){
        TROG_DEBUG("cb-connect");
        if(!ec) {
            p_internal_write_headers(write_headers_cb);
        } else {
            m_response_handler(ec, m_rdr);
        }
    });
}
void Request::p_internal_write_headers(WriteHeadersCallbackType write_headers_cb)
{
    p_assert_not_headers_written();
    p_add_chunked_encoding_header();
    p_prep_write_complete_headers();
    p_check_connected_before_write_headers(write_headers_cb);

    m_wrtr->async_write_headers(m_current_request, [this, write_headers_cb](Marvin::ErrorType err)
    {
        m_headers_written = true;
        write_headers_cb(err);
    });
}

void Request::asyncWriteTrailers(WriteHeadersCallbackType cb)
{
    p_assert_not_trailers_written();
    p_assert_trailers_permitted();
    // trailers only permitted when in chunked mode and headers had a TE
    m_wrtr->async_write_trailers(m_current_request, [this, cb](Marvin::ErrorType err)
    {
        m_trailers_written = true;
        cb(err);
    });
}
} // namespace
