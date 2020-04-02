//
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
#include <marvin/http/headers_v2.hpp>                  // for Headers, Head...
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
    m_current_request->setHeader(Marvin::Http::HeadersV2::Host, m_server+":"+m_port);
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

//Request::Request(boost::asio::io_service& io, std::string url)
//: _io(io), _url(url)
//{
//    setupUrl(url);
//}

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
    LogInfo("");
}
/*!--------------------------------------------------------------------------------
* implement set functions
*--------------------------------------------------------------------------------*/
void Request::setMethod(HttpMethod method)
{
    p_test_good_to_go();
    m_current_request->setMethod(method);
}
void Request::setUrl(std::string uri)
{

}
void Request::setPath(std::string path)
{
    p_test_good_to_go();
    m_current_request->setPath(path);
}
void Request::setVersion(int major, int minor)
{
    p_test_good_to_go();
    m_current_request->setHttpVersMajor(major);
    m_current_request->setHttpVersMinor(minor);
}
void Request::setHeaders(Marvin::Http::Headers headers)
{
    p_test_good_to_go();

}
void Request::setHeader(std::string key, std::string value)
{
    p_test_good_to_go();
    m_current_request->setHeader(key, value);
}
void Request::setTrailers(Marvin::Http::Headers trailers)
{
    p_test_good_to_go();
}
void Request::setTrailer(std::string key, std::string value)
{
    p_test_good_to_go();
    m_current_request->setTrailer(key, value);
}
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

/*!--------------------------------------------------------------------------------
* implement connect
*--------------------------------------------------------------------------------*/
void Request::asyncConnect(std::function<void(Marvin::ErrorType& err)> cb)
{
    LogInfo("", (long)this);

    auto f = [this, cb](Marvin::ErrorType& ec, ISocket* c) {
        std::string er_s = Marvin::make_error_description(ec);
        LogInfo(" conn", (long)m_conn_shared_ptr.get(), " er: ", er_s);
        if(!ec) {

        } else {
            m_conn_shared_ptr = nullptr;
        }
        cb(ec);
    };
    m_conn_shared_ptr->asyncConnect(f);
}
//--------------------------------------------------------------------------------
// asyncWriteHeaders
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
    // m_wrtr->asyncWriteHeaders(m_current_request, [this, cb](Marvin::ErrorType err){
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
    LogInfo("", (long)this);
    using namespace Marvin;
    std::function<void(ErrorType& err)> h = [](ErrorType& err) {

    };
    asyncConnect([this, write_headers_cb](Marvin::ErrorType& ec){
        LogDebug("cb-connect");
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

    m_wrtr->asyncWriteHeaders(m_current_request, [this, write_headers_cb](Marvin::ErrorType err){
        m_headers_written = true;
        write_headers_cb(err);
    });
}

void Request::asyncWriteTrailers(WriteHeadersCallbackType cb)
{
    p_assert_not_trailers_written();
    p_assert_trailers_permitted();
    // trailers only permitted when in chunked mode and headers had a TE
    m_wrtr->asyncWriteTrailers(m_current_request, [this, cb](Marvin::ErrorType err){
        m_trailers_written = true;
        cb(err);
    });
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
        p_check_connected_before_internal_write_message();
        return;
        int content_length = m_body_mbuffer_sptr->size();
        m_current_request->setHeader(Http::HeadersV2::ContentLength, std::to_string(content_length));
        m_wrtr->asyncWrite(m_current_request, m_body_mbuffer_sptr, [this, cb](Marvin::ErrorType err){
            cb(err);
        });
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
    LogInfo("", (long)this);
    using namespace Marvin;
    this->asyncConnect([this, body_chunk_chain_sptr, cb](Marvin::ErrorType& ec){
        LogDebug("cb-connect");
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

//--------------------------------------------------------------------------------
// start sequence of functions to write a the headers and a single body buffer.
//--------------------------------------------------------------------------------
void Request::p_internal_write_body_chunk(Marvin::BufferChainSPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb)
{

}

//--------------------------------------------------------------------------------
// start sequence of functions to read response
//--------------------------------------------------------------------------------
void Request::p_read_response_headers()
{
    this->m_rdr->readHeaders([this](Marvin::ErrorType ec2){
        auto resp_str = m_rdr->str();
        auto bdy_str = m_rdr->getContent()->to_string();
        if (!ec2) {
            // call onHeaders
            // setup read of body unless content length == 0
            if (this->m_rdr->hasHeader(Marvin::Http::HeadersV2::ContentLength)) {
                std::string clstr = this->m_rdr->getHeader(Marvin::Http::HeadersV2::ContentLength);
                if (clstr != "0") {
                    p_read_response_body();
                } else {
                    p_response_complete();
                }
            }
        } else {
            // call on error handler
            LogError("error on read headers");
            p_response_error(ec2);
        }
    });

}
void Request::p_read_response_body()
{
    this->m_rdr->readBody([this](Marvin::ErrorType err, Marvin::BufferChainSPtr buf_sptr){
        auto ex = err.value();
        if(err) {
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
void Request::p_read_response_handle_buffer(Marvin::BufferChainSPtr buf_sptr)
{

}
void Request::p_response_complete()
{

}
void Request::p_response_error(Marvin::ErrorType err)
{

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
//    _connection = nullptr;
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
    msg->setHeader(Marvin::Http::HeadersV2::ContentLength, std::to_string(len));
}
/*!--------------------------------------------------------------------------------
* implement helper functions
*--------------------------------------------------------------------------------*/
void Request::p_test_good_to_go()
{
    if(! m_conn_shared_ptr) {
        throw "Request::tryng to use m_conn_shared_ptr which is null";
    }
}
// check all the required headers are in place and add
// those that are done automaticaly
// must have either content-length or chunked encoding - throw if not
// add host header
void Request::p_prep_write_complete_headers()
{
    using namespace Marvin::Http;
    if(!m_current_request->hasHeader(HeadersV2::ContentLength)) {
        if (!m_current_request->hasHeader(HeadersV2::TransferEncoding)) {
            throw "Request::p_prep_write_complete_headers - no content length or chunked header";
        } else {
            auto te = m_current_request->getHeader(HeadersV2::TransferEncoding);
            if (te != "chunked") {
                throw "Request::p_prep_write_complete_headers - no content-length header and transfer-encoding header is not chunked";
            }
        }
    }
    m_current_request->setHeader(HeadersV2::Host, m_server+":"+m_port);
}

void Request::p_create_rdr_wrtr()
{
    p_test_good_to_go();
    this->m_rdr = std::make_shared<MessageReader>(m_io, m_conn_shared_ptr);
    this->m_wrtr = std::make_shared<MessageWriter>(m_io, m_conn_shared_ptr);
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
