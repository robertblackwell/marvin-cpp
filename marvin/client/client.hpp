
#ifndef marvin_client_client_hpp
#define marvin_client_client_hpp
#include <functional>
#include <istream>
#include <string>
#include <memory>
#include <boost/asio/io_service.hpp>
#include <marvin/http/header_fields.hpp>
#include <marvin/http/uri.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include <marvin/message/message_writer.hpp>
#include <marvin/error/marvin_error.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
namespace boost { namespace asio { namespace ip { class tcp; } } }
namespace boost { namespace system { class error_code; } }  // lines 19-19

namespace Marvin {
/**
* \defgroup Client
* \brief A class to act as an http or https client./
*/
class Client;  // lines 21-21

using ::boost::asio::ip::tcp;
using ClientSPtr = std::shared_ptr<Client>;
using ClientUPtr = std::unique_ptr<Client>;
/**
* \ingroup client
* \brief This is the type signature of callbacks that receive a fully or partially complete
* response
*/
using ResponseHandlerCallbackType = std::function<void(Marvin::ErrorType& err, MessageBase::SPtr msg)>;

/**
* \ingroup client
* \brief This is the type signature of callbacks that receive chunks of body data
*/
using ClientDataHandlerCallbackType = std::function<void(Marvin::ErrorType& err, Marvin::BufferChain buf_chain)>;

/**
* determines whether a new MessageReader and MessageWriter
* are allocated for each new roundtrip. 
*/
#define RDR_WRTR_ONESHOT 1

/**
* \ingroup client
* \brief This class implements an http client that can send a request message and wait for a response;
*  and can manage one or more such `round trips` to a server.
*
*/
class Client
{
public:
    using ResponseHandler = ResponseHandlerCallbackType;
    using ConnectHandler = std::function<void()>;
    using WriteHeadersHandler = MessageWriter::WriteHeadersHandler;
    using WriteBodyDataHandler = MessageWriter::WriteBodyDataHandler;

#pragma mark - constructors and destructors
    /**
     * Create a client that is not connected; but provide the info it needs
     * to try and establish a connection; if connectiing through a proxy the host/port
     * should point at the proxy.
     * \deprecated
     */
    Client(boost::asio::io_service& io, std::string scheme, std::string server, std::string port);
    
    /**
     * Construct a not-yet-connected client instance and provide target host
     * information in the form of a Marvin::Uri instance.
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
    
    MessageBase::SPtr  getResponse();
    
    void set_url(std::string url);
    void set_content(std::string& contentStr);
    
    /**
    * Connects to the target host. If the scheme is https also includes a handshake.
    * If the ISocket is not already connected this method will becalled when the first write call is issued
    */
    void async_connect(std::function<void(ErrorType& err)> cb);

    /**
    * \brief Writes the complete message to the connected host including body and trailers (if there are any).
    *
    * If not connected will perform a connect before transmitting the message. Will wait for a response
    * and call the onResponseHandler CB when the response is complete.
    *
    * If the request is to have a body it must be already attached to the MessageBase.
    * A content_length header will be attached by the client.
    *
    * If the request is to have NO body use this method with the requestMessage.body == nullptr;
    */
    void async_write(Marvin::MessageBaseSPtr requestMessage,  ResponseHandler cb);
    /**
    * The following are three ways to add a body to a request. The first two take a string or
    * a shared pointer to an ContigBuffer - these are contiguous buffers and can be sent with a single
    * async_write
    */
    void async_write(Marvin::MessageBaseSPtr requestMessage,  std::string& body_str, ResponseHandler cb);
    void async_write(Marvin::MessageBaseSPtr requestMessage,  Marvin::ContigBuffer::SPtr body_sptr, ResponseHandler cb);
    void async_write(Marvin::MessageBaseSPtr requestMessage,  Marvin::BufferChain::SPtr chain_sptr, ResponseHandler cb);

    /**
    * close - called to signal that connection::close has been received
    * and that the net round trip should use a different connection
    * for the next message. This invalidates m_connection, m_rdr, m_wrtr
    *
    * @TODO - not sure we need this - just delete the client object
    */
    void close();
protected:
    void p_internal_connect();
    void p_internal_write();
    void p_async_write(Marvin::MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    void p_set_content_length();
    
    std::string m_url;
    std::string m_uri;
    std::string m_scheme;
    std::string m_server;
    std::string m_host;
    std::string m_host_with_port;
    std::string m_port;
    std::string m_path;

    boost::asio::io_service&          m_io;
    Marvin::MessageBaseSPtr           m_current_request;
    Marvin::ContigBuffer::SPtr        m_body_mbuffer_sptr;
    MessageWriter::SPtr               m_wrtr;
    MessageReaderV2::SPtr             m_rdr;
    ISocketSPtr                       m_conn_shared_ptr;
    ResponseHandlerCallbackType       m_response_handler;

};
} // namespaace
#endif
