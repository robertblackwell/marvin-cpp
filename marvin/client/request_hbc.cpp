/**
*
* This file contains the implemntation of the sequence of steps required
* to write the request headers plus a single body chunk
*/
#include <marvin/client/request.hpp>
#include <cassert>                                      // for assert
#include <istream>                                      // for string
#include <marvin/connection/socket_factory.hpp>         // for socketFactory
#include <trog/trog.hpp>  // for LogInfo, LogD...
#include <marvin/message/message_reader.hpp>            // for MessageReader
#include <memory>                                       // for operator!=
#include <string>                                       // for to_string
#include <boost/asio/streambuf.hpp>                     // for streambuf
#include <cert/error.hpp>                               // for MARVIN_THROW
#include <marvin/http/headers_v2.hpp>                  // for Headers, Head...
#include <marvin/http/uri.hpp>                          // for Uri
#include <marvin/message/message_writer.hpp>            // for MessageWriter
#include <trog/trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }

namespace Marvin {
using ::boost::asio::ip::tcp;
using ::boost::system::error_code;
using ::boost::asio::io_service;
using ::boost::asio::streambuf;

//--------------------------------------------------------------------------------
// start sequence of functions to write a the headers and a single body buffer.
// hbc = headers_and_body_chunk
//--------------------------------------------------------------------------------
void Request::p_hbc_check_connected(BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb)
{
    if(m_is_connected) {
        p_hbc_write(body_chunk_chain_sptr, cb);
    } else {
        p_hbc_connect(body_chunk_chain_sptr, cb);
    }
}
void Request::p_hbc_connect(::Marvin::BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb)
{
    TROG_INFO("", (long)this);
    using namespace Marvin;
    this->asyncConnect([this, body_chunk_chain_sptr, cb](Marvin::ErrorType& ec){
        TROG_DEBUG("cb-connect");
        if(!ec) {
            p_hbc_write(body_chunk_chain_sptr, cb);
        } else {
            m_response_handler(ec, m_rdr);
        }
    });
}
void Request::p_hbc_write(BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType write_headers_cb)
{
    p_assert_not_headers_written();
    p_add_chunked_encoding_header();
    p_prep_write_complete_headers();

    m_wrtr->asyncWriteHeaders(m_current_request, [this, write_headers_cb](Marvin::ErrorType err){
        m_headers_written = true;
        write_headers_cb(err);
    });
}
} // namespace