
#ifndef client_hpp
#define client_hpp
/**
* \defgroup Client
*/

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/http/http_header.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/message/message_reader.hpp>
#include <marvin/connection/tcp_connection.hpp>
#include <marvin/message/marvin_uri.hpp>

using boost::asio::ip::tcp;
class Client;
/// \ingroup Client
using ClientSPtr = std::shared_ptr<Client>;
/// \ingroup Client
using ClientUPtr = std::unique_ptr<Client>;
/**
* \ingroup Client
* \brief This is the type signature of callbacks that receive a fully or partially complete
* response
*/
using ResponseHandlerCallbackType = std::function<void(Marvin::ErrorType& err, MessageReaderSPtr msg)>;

/**
* \ingroup Client
* \brief This is the type signature of callbacks that receive chunks of body data
*/
using ClientDataHandlerCallbackType = std::function<void(Marvin::ErrorType& err, Marvin::BufferChain buf_chain)>;

/**
* determines whether a new MessageReader and MessageWriter
* are allocated for each new roundtrip. 
*/
#define RDR_WRTR_ONESHOT 1

/**
* \ingroup Client
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
* Two constructors are provided
*   -   one will construct a non-connected client for which an explicit or implicit
*       connect call will be required.
*   -   the other creates a client with an established connection.
*
*/
class Client
{
public:
#pragma mark - constructors and destructors
    /**
     * \brief Create a client that is not connected; but provide the info it needs
     * to try and establish a connection; if connectiing through a proxy the host/port
     * should point at the proxy.
     *
     * @param io - an io service
     * @param scheme of type HttpHeader::SchemeType
     * @param server - a string like google.com
     * @param port - a string like "443"
     * \deprecated
     */
    Client(boost::asio::io_service& io, Marvin::Http::Headers::SchemeType scheme, std::string server, std::string port);
    
    /**
     * @brief Conosttruct an not-yet-connected client instance and provide target host
     * information in the form of a Marvin::Uriu instance.
     *
     * @param io - an io service
     * @param uri - Marvin::Uri - contains details of the host to which the instance is to connect.
     */
    Client(boost::asio::io_service& io, Marvin::Uri uri);
    /**
    * Create a client with an established connection. In this case a call to
    * connect will fail with an exception - as it is a logic error
    */
    Client(boost::asio::io_service& io, ISocketSPtr conn);

    Client(const Client& other) = delete;
    Client& operator=(const Client&) = delete;
    
    ~Client();
    
#pragma mark - getters and setters
    
    MessageReaderSPtr  getResponse();
    
    void setUrl(std::string url);    
    void setContent(std::string& contentStr);
    
#pragma mark - event registration
    /**
    * Sets a handler to be called when a complete response message is received or if
    * an error occurs while receiving a response message.
    */
    void setOnResponse(ResponseHandlerCallbackType cb );

    /**
    * Sets a handler to be called when all headers have been received or if
    * an error occurs while receiving a response message headers.
    *
    * optional
    */
    void setOnHeaders(ResponseHandlerCallbackType cb);

    /**
    * Sets a handler to be called when a chunk of body data is available.
    *
    * optional
    */
    void setOnData(ClientDataHandlerCallbackType cb);

#if 0 // these are not yet implemented
    /**
    * Sets a handler to be called when all body data has been received
    *
    * optional
    */
    void setOnComplete(ErrorOnlyCallbackType cb);
#endif

 #pragma mark - io methods
 
    /**
    * Connects to the target host
    */
    void asyncConnect(ErrorOnlyCallbackType cb);
    
    /**
    * \brief Writes the complete message to the connected host inlcuding body and trailers (if there are any).
    *
    * If not connected will perform a connect before transmitting the message. Will wait for a response
    * and call the onResponseHandler CB when the response is complete.
    *
    * If the request is to have a body it must be already attached to the MessageInterface.
    * A content_length header will be attached by the client.
    *
    * If the request is to have NO body use this method with the requestMessage.body == nullptr;
    */
    void asyncWrite(Marvin::Http::MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    /**
    * The following are three ways to add a body to a request. The first two take a string or
    * a shared pointer to an MBuffer - these are contiguous buffers and can be sent with a single
    * asyncWrite
    */
    void asyncWrite(Marvin::Http::MessageBaseSPtr requestMessage,  std::string& body_str, ResponseHandlerCallbackType cb);
    void asyncWrite(Marvin::Http::MessageBaseSPtr requestMessage,  Marvin::MBufferSPtr body_sptr, ResponseHandlerCallbackType cb);
    void asyncWrite(Marvin::Http::MessageBaseSPtr requestMessage,  Marvin::BufferChainSPtr chain_sptr, ResponseHandlerCallbackType cb);

    /**
    * Sends the first line and headers of the request message only
    * and expects any body data to be sent using asyncWriteBodyData
    * in which case the body will be "chunk" encoded and the headers
    * will be completed to indicate this.
    *
    * Dont use this method IF there is no body data, use asyncWrite
    */
    void asyncWriteHeaders(Marvin::Http::MessageBaseSPtr requestMessage, WriteHeadersCallbackType cb);
    
    /**
    * Transmits a block of body data - the data should NOT be chunk encode
    * that will be done within the call.
    *
    * @param dataBuffer (type TBD) can also be nullptr
    * @param last bool - signals this is the last dataBuffer and that the
    *                   chunk trailer should be generated.
    * @param cb     -   handler to be called when operation complete.
    *
    * @note - asyncWriteBodyData( nullptr, true, ....) is the normal way to
    *   -   signal end of data, as the caller may not know its e-o-d until
    *       after the last buffer is sent. Such a call still sends data
    *       on the connection
    *
    *       - the chunk encoding trailer.
    *
    */
    void asyncWriteBodyData(void* dataBuffer, bool last, WriteBodyDataCallbackType cb);
    
    /**
    * not yet implemented
    *
    * This method should only be used if the transmission was started with a
    * call to asyncWriteHeaders. The method asyncWrite handles trailers automatically.
    *
    * Sends the trailers that are present in the requestMessage - trailers can be added
    * to the message AFTER transmission has started as the internals of this class
    * do not inspect the trailers until this call nor does it keep a copy of the
    * original request
    */
    void asyncWriteTrailers(Marvin::Http::MessageBaseSPtr requestMessage,  AsyncWriteCallbackType cb);
    
    /**
    * Called to signal end-of-message. This function will
    * (if necessary) add the chunk-encoding trailer.
    */
    void end();
    
    /**
    * close - called to signal that connection::close has been received
    * and that the net round trip should use a different connection
    * for the next message. This invalidates _connection, _rdr, _wrtr
    *
    * @TODO - not sure we need this - just delete the client object
    */
    void close();
    
#pragma mark - friend utility functions
   
    friend std::string traceClient(Client& client);
    friend std::string traceRequestMessage(Marvin::Http::MessageBase& request);

protected:
    void internalConnect();
    void internalWrite();

    void _async_write(Marvin::Http::MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    void putHeadersStuffInBuffer();
    void setContentLength();
    
    std::string m_url; // resource locator
    std::string m_uri; // really path
    std::string m_scheme;    // http or https
    std::string m_server;  // this is without the port localhost for example
    std::string m_host;           // as used in the headers eg localhost:9991 for example
    std::string m_host_with_port; // as used in the headers eg localhost:9991 for example
    std::string m_port;
    std::string m_path;

    boost::asio::io_service&                        m_io;
    Marvin::Http::MessageBaseSPtr                   m_current_request;
    Marvin::MBufferSPtr                             m_body_mbuffer_sptr;
    MessageWriterSPtr                               m_wrtr;
    MessageReaderSPtr                               m_rdr;
    ISocketSPtr                                     m_conn_shared_ptr;
    ResponseHandlerCallbackType                     m_response_handler;
    ResponseHandlerCallbackType                     m_on_headers_handler;
    ClientDataHandlerCallbackType                   m_on_data_handler;
//    bool        _oneTripOnly;
//    
//    std::string _service;   //used by boost for resolve and connnect http/https or a port number
//    std::string _server;    // as used in boost resolve/connect WITHOUT port number
    
};
#endif

