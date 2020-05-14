//
#include <marvin/client/request.hpp>
#include <cassert>                                      // for assert
#include <istream>                                      // for string
#include <marvin/connection/socket_factory.hpp>         // for socketFactory
#include <marvin/configure_trog.hpp>  // for LogInfo, LogD...
#include <marvin/message/message_reader.hpp>            // for MessageReader
#include <memory>                                       // for operator!=
#include <string>                                       // for to_string
#include <boost/asio/streambuf.hpp>                     // for streambuf
#include <cert/error.hpp>                               // for MARVIN_THROW
#include <marvin/http/headers_v2.hpp>                  // for Headers, Head...
#include <marvin/http/uri.hpp>                          // for Uri
#include <marvin/message/message_writer.hpp>            // for MessageWriter
#include <marvin/error_handler/error_handler.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }

namespace Marvin {
using ::boost::asio::ip::tcp;
using ::boost::system::error_code;
using ::boost::asio::io_service;
using ::boost::asio::streambuf;

Request::Request(
    boost::asio::io_service& io, 
    std::string scheme, 
    std::string server, 
    std::string port
): m_io(io), m_scheme(scheme), m_server(server), m_port(port), m_headers_written(false), m_trailers_written(false)
{
    std::cout << "Constructor" << std::endl;
    m_conn_shared_ptr = socketFactory(m_io, m_scheme, m_server, m_port);
    p_create_rdr_wrtr();
    m_current_request->header(Marvin::HeadersV2::Host, m_server+":"+m_port);
    m_is_connected = false;
}
Request::Request(
    boost::asio::io_service& io, 
    Marvin::Uri uri
): m_io(io), m_scheme(uri.scheme()), m_server(uri.server()), m_port(std::to_string(uri.port()))
        , m_headers_written(false), m_trailers_written(false)
{
    assert(false); // need to pass the Url() to connection
    m_conn_shared_ptr = socketFactory(m_io);
    p_create_rdr_wrtr();
    m_is_connected = false;

}

Request::Request(
    boost::asio::io_service& io, 
    ISocketSPtr conn
): m_io(io), m_headers_written(false), m_trailers_written(false)
{
    m_conn_shared_ptr = conn;
    p_create_rdr_wrtr();
    m_is_connected = true;

}


Request::~Request()
{
    TROG_INFO("");
}
/*!--------------------------------------------------------------------------------
* implement set functions
*--------------------------------------------------------------------------------*/
void Request::method(HttpMethod method)
{
    p_test_good_to_go();
    m_current_request->method(method);
}
void Request::setUrl(std::string uri)
{

}
void Request::target(std::string path)
{
    p_test_good_to_go();
    m_current_request->target(path);
}
void Request::setVersion(int major, int minor)
{
    p_test_good_to_go();
    m_current_request->version_major(major);
    m_current_request->version_minor(minor);
}
void Request::header(Marvin::HeadersV2 headers)
{
    p_test_good_to_go();

}
void Request::header(std::string key, std::string value)
{
    p_test_good_to_go();
    m_current_request->header(key, value);
}

/*!--------------------------------------------------------------------------------
* implement connect
*--------------------------------------------------------------------------------*/
void Request::asyncConnect(std::function<void(Marvin::ErrorType& err)> cb)
{
    TROG_INFO("", (long)this);

    auto f = [this, cb](Marvin::ErrorType& ec, ISocket* c) {
        std::string er_s = Marvin::make_error_description(ec);
        TROG_INFO(" conn", (long)m_conn_shared_ptr.get(), " er: ", er_s);
        if(!ec) {

        } else {
            m_conn_shared_ptr = nullptr;
        }
        cb(ec);
    };
    m_conn_shared_ptr->asyncConnect(f);
}

//----------------------------------------------------------------------------------------------
// asyncWriteBodyData - requires that headers NOT already sent and will force chunked encodiing
//-----------------------------------------------------------------------------------------------
void Request::asyncWriteBodyData(std::string& body_chunk, WriteBodyDataCallbackType cb)
{
    //assert body_chunk != ""
    m_body_mbuffer_sptr = Marvin::MBuffer::makeSPtr(body_chunk);
    BufferChainSPtr body_chunk_chain_sptr = BufferChain::makeSPtr(body_chunk);
    if (p_test_not_headers_written()) {
        p_add_chunked_encoding_header();
        p_prep_write_complete_headers();
        p_hbc_check_connected(body_chunk_chain_sptr, cb);
        // write headers
        // then send the buffer as chunked encoding
    } else {
        // send the buffer chunk encoded
    }
}
void Request::asyncWriteBodyData(Marvin::MBufferSPtr mbuf_sptr, WriteBodyDataCallbackType cb)
{
    // assert body_chunk_ptr != nullptr and not empty
    m_body_mbuffer_sptr = mbuf_sptr;
    BufferChainSPtr body_chunk_chain_sptr = BufferChain::makeSPtr(mbuf_sptr);
    if (p_test_not_headers_written()) {
        p_add_chunked_encoding_header();
        p_prep_write_complete_headers();
        p_hbc_check_connected(body_chunk_chain_sptr, cb);
        // write headers
        // then send the buffer as chunked encoding
    } else {
        // send the buffer chunk encoded
    }
}
void Request::asyncWriteBodyData(Marvin::BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb)
{
    // assert body_chunk_chain_ptr != nullptr and not empty
    m_body_mbuffer_sptr = body_chunk_chain_sptr->amalgamate();
    if (p_test_not_headers_written()) {
        p_add_chunked_encoding_header();
        p_prep_write_complete_headers();
        p_hbc_check_connected(body_chunk_chain_sptr, cb);
        // write headers
        // then send the buffer as chunked encoding
    } else {
        // send the buffer chunk encoded
    }
}
//--------------------------------------------------------------------------------
// asyncWriteLastBodyData - allowable combinations
//      if headers have been sent already a chunked encoding header should have been included
//          as sending headers without a content-length should force as chunked header
//          hence start the chunk encoding process for this body buffer
//
//      if headers NOT already sent compute the length of the body
//          add a content-length header of that size
//          link the body buffer to m_current_request
//          and sent the entire message
//--------------------------------------------------------------------------------
void Request::asyncWriteLastBodyData(std::string& body_chunk_str, WriteBodyDataCallbackType  cb)
{
    m_body_mbuffer_sptr = Marvin::MBuffer::makeSPtr(body_chunk_str);
    if(p_test_not_headers_written()) {
        // compute buffer length, add content-length header
        // send the entire message
        MBufferSPtr mbuf_sptr = MBuffer::makeSPtr(body_chunk_str);
        p_msg_check_connected(m_current_request, mbuf_sptr, cb);
    } else {
        //  if buffer not empty send the buffer chunk encoded
        //  send chunk encoding final sequence after the buffer is gone
    }
}
void Request::asyncWriteLastBodyData(Marvin::MBufferSPtr body_chunk_sptr, WriteBodyDataCallbackType  cb)
{
    m_body_mbuffer_sptr = body_chunk_sptr;
    if(p_test_not_headers_written()) {
        // compute buffer length, add content-length header
        // send the entire message
        m_wrtr->asyncWrite(m_current_request, body_chunk_sptr, [this, cb](Marvin::ErrorType err){
            cb(err);
        });
    } else {
        //  if buffer not empty send the buffer chunk encoded
        //  send chunk encoding final sequence after the buffer is gone
    }
}
void Request::asyncWriteLastBodyData(Marvin::BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb)
{
    m_body_mbuffer_sptr = body_chunk_chain_sptr->amalgamate();
    if(p_test_not_headers_written()) {
        // compute buffer length, add content-length header
        // send the entire message
        m_wrtr->asyncWrite(m_current_request, body_chunk_chain_sptr, [this, cb](Marvin::ErrorType err){
            cb(err);
        });
    } else {
        //  if buffer not empty send the buffer chunk encoded
        //  send chunk encoding final sequence after the buffer is gone
    }
}

//-----------------------------------------------------------------------------------------
// utility functions to propgate events
//------------------------------------------------------------------------------------------
void Request::setOnResponse(ResponseHandlerCallbackType cb )
{
    m_on_response_complete_cb = cb;
}
void Request::setOnHeaders(ResponseHandlerCallbackType cb)
{
    m_on_headers_complete_cb = cb;
}
void Request::setOnData(RequestDataHandlerCallbackType cb)
{
    m_on_rdata_cb = cb;
}
void Request::setOnError(ErrorOnlyCallbackType cb)
{
    m_on_error_cb = cb;
}
void Request::p_resp_on_error(Marvin::ErrorType& ec2)
{
    if (!this->m_on_error_cb) {
        MARVIN_THROW("on_error event handler not set");
    }
    m_on_error_cb(ec2);
}
void Request::p_resp_on_headers(Marvin::ErrorType& ec2, MessageReaderSPtr msg)
{
    if (!this->m_on_headers_complete_cb) {
        MARVIN_THROW("on_response_complete event handler not set");
    }
    m_on_headers_complete_cb(ec2, msg);
}
void Request::p_resp_on_data(Marvin::ErrorType& err, BufferChainSPtr buf)
{
    if (!this->m_on_rdata_cb) {
        MARVIN_THROW("on_response_complete event handler not set");
    }
    Marvin::ErrorType err2 = err;
    m_on_rdata_cb(err2, buf);
}
void Request::p_resp_on_complete(Marvin::ErrorType& ec2, MessageReaderSPtr msg)
{
    if (!this->m_on_response_complete_cb) {
        MARVIN_THROW("on_response_complete event handler not set");
    }
    m_on_response_complete_cb(ec2, msg);
}

//-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Request::p_write_error()
{

}
void Request::close()
{
    m_conn_shared_ptr->close();
    m_rdr = nullptr;
    m_wrtr = nullptr;
    m_conn_shared_ptr = nullptr;
}
void Request::end()
{
}

MessageReaderSPtr Request::getResponse()
{
    return m_rdr;
}
void Request::p_set_content_length()
{
    long len = 0;
    MessageBaseSPtr msg = m_current_request;
    if( m_body_mbuffer_sptr != nullptr ) {
        len = m_body_mbuffer_sptr->size();
    }
    msg->header(Marvin::HeadersV2::ContentLength, std::to_string(len));
}
/*!--------------------------------------------------------------------------------
* implement helper functions
*--------------------------------------------------------------------------------*/
void Request::p_test_good_to_go()
{
    if(! m_conn_shared_ptr) {
        MARVIN_THROW("Request::tryng to use m_conn_shared_ptr which is null");
    }
}
// check all the required headers are in place and add
// those that are done automaticaly
// must have either content-length or chunked encoding - MARVIN_THROW if not
// add host header
void Request::p_prep_write_complete_headers()
{
    auto hopt_cl = m_current_request->header(HeadersV2::ContentLength);
    auto hopt_te = m_current_request->header(HeadersV2::TransferEncoding);

    if(! hopt_cl ) {
        if (! hopt_te ) {
            MARVIN_THROW("Request::p_prep_write_complete_headers - no content length or chunked header");
        } else {
            auto te = hopt_te.get();
            if (te != "chunked") {
                MARVIN_THROW("Request::p_prep_write_complete_headers - no content-length header and transfer-encoding header is not chunked");
            }
        }
    }
    m_current_request->header(HeadersV2::Host, m_server+":"+m_port);
}

void Request::p_create_rdr_wrtr()
{
    p_test_good_to_go();
    this->m_rdr = std::make_shared<MessageReader>(m_conn_shared_ptr);
    this->m_wrtr = std::make_shared<MessageWriter>(m_conn_shared_ptr);
    this->m_current_request =std::make_shared<MessageBase>();
}

bool Request::p_test_not_headers_written()
{
    return ! m_headers_written;
}
void Request::p_add_chunked_encoding_header()
{

}
void Request::p_assert_not_headers_written()
{
    assert(!m_headers_written);
}
void Request::p_assert_not_trailers_written()
{
    assert(!m_trailers_written);
}
void Request::p_assert_trailers_permitted()
{
    assert(true);
}
} // namespace