
#ifndef marvin_client_request_hpp
#define marvin_client_request_hpp
#include <functional>                              // for function
#include <istream>                                 // for string
#include <marvin/http/headers_v2.hpp>             // for Headers, Headers::...
#include <marvin/http/uri.hpp>                     // for Uri
#include <marvin/message/message_reader.hpp>       // for MessageReaderSPtr
#include <marvin/message/message_writer.hpp>       // for MessageWriterSPtr
#include <memory>                                  // for shared_ptr, unique...
#include <boost/asio/io_service.hpp>               // for io_service
#include <marvin/callback_typedefs.hpp>            // for AsyncWriteCallback...
#include <marvin/connection/socket_interface.hpp>  // for ISocketSPtr
#include <marvin/error/marvin_error.hpp>           // for ErrorType
#include <marvin/http/message_base.hpp>            // for MessageBaseSPtr
#include <memory>                                       // for operator!=
#include <string>                                       // for to_string
#include <boost/asio/io_service.hpp>                    // for io_service
#include <marvin/buffer/buffer.hpp>               // for BufferChain
#include <marvin/callback_typedefs.hpp>                 // for ErrorOnlyCall...
#include <marvin/connection/socket_interface.hpp>       // for ISocketSPtr
#include <marvin/error/marvin_error.hpp>                // for ErrorType
#include <marvin/http/message_base.hpp>                 // for MessageBaseSPtr

namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }  // lines 19-19

namespace Marvin {
class Request;  // lines 21-21

using ::boost::asio::ip::tcp;
using RequestSPtr = std::shared_ptr<Request>;
using RequestUPtr = std::unique_ptr<Request>;
using ResponseHandlerCallbackType = std::function<void(Marvin::ErrorType& err, MessageReaderSPtr msg)>;

using RequestDataHandlerCallbackType = std::function<void(Marvin::ErrorType& err, Marvin::BufferChain::SPtr buf_chain)>;

#define REQUEST_RDR_WRTR_ONESHOT 1

/**
* \ingroup client
* \brief This class implements an http client that can send a request message and wait for a response;
*  and can manage one or more such `round trips` to a server.
*
* This class connects to a server, then
*  -   sends a message (MessageBase instance) to the server, and waits for a response in the
*      form of a MessageReader instance.  This is a `round trip`.
*  -   The connection is only established once and multiple independent round trips can be
*      performed on that connection.
*  -   each round trip requires the caller to provide an request in the form of a MessageBase
*      instance and receives a response in the form of a MessageReadeer instance.
*  -   the class can perform round trips directly to a server or via a proxy
*  -   the request (MessageBase) provided by the caller must have a uri and host header
*      that is compatible with the connected host and with the use or non use of a proxy.
*  -   the Client instance DOES NOT check or modify the request MessageBase instance in any
*      way(see exception below), getting the request correct is the callers responsibility.
*  -   exception to the previous point is the content length header which some of the writeAsync
*      set.
*  -   @note the caller is responsible for managing connection keep-alive and close
*
* Two modes of transmission operation are provided for:
*
*   -   send the entire request in a single call using asyncWrite. This mode is best when
*       either there is NO message body or the entire message body is available at the time
*       the request transmission is started. In this mode the handler provided to asyncWrite
*       is passed a complete response including any body data.
*
*   -   send the request/message piecemeal - headers, followed by multiple chunks of body, followed by trailers.
*       This mode is intended to handle streaming of the body data to avoid latency or buffering large body. In this
*       mode the handler for the incoming response message is provide by the setOnResponseHandler. The response handler
*       should be set before any transmission is started. A complete response including body data is provided
*       to this handler.
*
* Constructors
*   -   two will construct a non-connected client for which an explicit or implicit
*       connect call will be required.
*   -   the other creates a client with an established connection.
*
* HTTPS - the interface does not yet provide a means of giving the Client a certificate bundle which is required
*       to establish a secure connection. The plan is to make the cert bundle always be explicit rather than
*       using the openssl/os default bundle
* Design note:
*   contemplating makiing the MessageBase instance internal to this class 
*/
class Request
{
public:
#pragma mark - constructors and destructors
    Request(boost::asio::io_service& io, std::string scheme, std::string server, std::string port);
    Request(boost::asio::io_service& io, Marvin::Uri uri);
    Request(boost::asio::io_service& io, ISocketSPtr conn);

    Request() = delete;
    Request(const Request& other) = delete;
    Request& operator=(const Request&) = delete;
    
    ~Request();
    // TODO not sure we need these
    MessageReaderSPtr  getResponse();
    void setContent(std::string& contentStr);
    
    void setOnResponse(ResponseHandlerCallbackType cb );
    void setOnHeaders(ResponseHandlerCallbackType cb);
    void setOnData(RequestDataHandlerCallbackType cb);
    void setOnError(ErrorOnlyCallbackType cb);

#if MARVIN_REQUEST_NOT_IMPLEMENTED // these are not yet implemented
    /**
    * Sets a handler to be called when all body data has been received
    *
    * optional
    */
    void setOnComplete(ErrorOnlyCallbackType cb);
    /**
    * Sets a handler for errors during the roundtrip
    *
    */
    void setOnError(ErrorOnlyCallbackType cb);
#endif
    void method(HttpMethod method);
    void setUrl(std::string url);

    void target(std::string path);
    void setVersion(int major, int minor);
    void header(Marvin::HeadersV2 headers);
    void header(std::string key, std::string value);

    void asyncWriteHeaders(WriteHeadersCallbackType cb);
    void asyncWriteTrailers(WriteHeadersCallbackType cb);

    void asyncWriteBodyData(std::string& body_str, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::ContigBuffer::SPtr body_sptr, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::BufferChain::SPtr chain_sptr, WriteBodyDataCallbackType cb);

    void asyncWriteLastBodyData(std::string& body_str, WriteBodyDataCallbackType  cb);
    void asyncWriteLastBodyData(Marvin::ContigBuffer::SPtr body_sptr, WriteBodyDataCallbackType  cb);
    void asyncWriteLastBodyData(Marvin::BufferChain::SPtr chain_sptr, WriteBodyDataCallbackType cb);
    void asyncConnect(std::function<void(Marvin::ErrorType& err)> cb);
    void end();
    
    /**
    * close - called to signal that connection::close has been received
    * and that the net round trip should use a different connection
    * for the next message. This invalidates m_connection, m_rdr, m_wrtr
    *
    * @TODO - not sure we need this - just delete the client object
    */
    void close();
    
#pragma mark - friend utility functions
   
    friend std::string traceRequest(Request& request);
    friend std::string traceRequestMessage(Marvin::MessageBase& request);

public:
    // internal paranoid tests    
    bool p_test_not_headers_written();
    void p_assert_not_headers_written();
    void p_assert_not_trailers_written();
    void p_assert_trailers_permitted();
    // utility methods
    void p_create_rdr_wrtr();
    void p_test_good_to_go();
    // write headers steps
    void p_check_connected_before_write_headers(WriteHeadersCallbackType write_headers_cb);
    void p_internal_connect_before_write_headers(WriteHeadersCallbackType write_headers_cb);
    void p_internal_write_headers(WriteHeadersCallbackType write_headers_cb);
    // steps in connect/write the request
    void p_put_header_stuff_in_buffer();
    void p_prep_write_complete_headers();
    void p_set_content_length();
    void p_add_chunked_encoding_header();
    // sequence for writing headers and a body chunk
    void p_hbc_check_connected(BufferChain::SPtr chain_sptr, WriteBodyDataCallbackType cb);
    void p_hbc_connect(BufferChain::SPtr chain_sptr, WriteBodyDataCallbackType cb);
    void p_hbc_write(BufferChain::SPtr chain_sptr, WriteBodyDataCallbackType cb);

    // steps in writing a body chunk.
    void p_internal_write_headers_and_body_chunk(Marvin::BufferChain::SPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb);
    void p_internal_write_body_chunk(Marvin::BufferChain::SPtr body_chunk_chain_sptr, WriteBodyDataCallbackType cb);

    // steps in writing a full message in one go
    void p_msg_check_connected(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb);
    void p_msg_connect(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb);
    void p_msg_write(MessageBaseSPtr msg, ContigBuffer::SPtr mbuf_sptr, WriteMessageCallbackType cb);
    // general write error handler
    void p_write_error();
    // @TODO need to determine role of this method
    void p_async_write(Marvin::MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    // steps in read response
    void p_read_response_headers();
    void p_read_response_body();
    void p_read_response_body_next();
    void p_read_response_handle_buffer(Marvin::BufferChain::SPtr buf_sptr);
    void p_response_complete();
    void p_response_error(Marvin::ErrorType err);

    // event handler utility funcs
    void p_resp_on_error(Marvin::ErrorType& ec2);
    void p_resp_on_headers(Marvin::ErrorType& ec2, MessageReaderSPtr msg);
    void p_resp_on_data(Marvin::ErrorType& err, BufferChain::SPtr buf);
    void p_resp_on_complete(Marvin::ErrorType& ec2, MessageReaderSPtr msg);

    ResponseHandlerCallbackType m_on_response_complete_cb;
    ResponseHandlerCallbackType m_on_headers_complete_cb;
    RequestDataHandlerCallbackType m_on_rdata_cb;
    ErrorOnlyCallbackType m_on_error_cb;

    bool m_headers_written;
    bool m_trailers_written;
    // std::string m_url; // resource locator
    // std::string m_uri; // really path
    std::string m_scheme;    // http or https
    std::string m_server;  // this is without the port localhost for example
    std::string m_host;           // as used in the headers eg localhost:9991 for example
    std::string m_host_with_port; // as used in the headers eg localhost:9991 for example
    std::string m_port;
    std::string m_path;

    boost::asio::io_service&                        m_io;
    Marvin::MessageBaseSPtr                   m_current_request;
    Marvin::ContigBuffer::SPtr                             m_body_mbuffer_sptr;
    MessageWriterSPtr                               m_wrtr;
    MessageReaderSPtr                               m_rdr;
    ISocketSPtr                                     m_conn_shared_ptr;
    bool                                            m_is_connected;
    ResponseHandlerCallbackType                     m_response_handler;
    ResponseHandlerCallbackType                     m_on_headers_handler;
    RequestDataHandlerCallbackType                   m_on_data_handler;
    
};
} // namespace
#endif

