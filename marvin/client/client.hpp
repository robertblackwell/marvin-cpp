//
//  client.hpp
//  all
//
//  Created by ROBERT BLACKWELL on 11/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef client_hpp
#define client_hpp


#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "bufferV2.hpp"
#include "http_header.hpp"
#include "message_writer_v2.hpp"
#include "message_reader_v2.hpp"
#include "tcp_connection.hpp"
#include "url.hpp"

using boost::asio::ip::tcp;
class Client;
typedef std::shared_ptr<Client> ClientSPtr;
typedef std::unique_ptr<Client> ClientUPtr;
/**
* This is the type signature of callbacks that receive a fully or partially complete
* response
*/
typedef std::function<void(Marvin::ErrorType& err, MessageReaderV2SPtr msg)> ResponseHandlerCallbackType;

/**
* This is the type signature of callbacks that receive chunks of body data
*/
typedef std::function<void(Marvin::ErrorType& err, BufferChain buf_chain)>  ClientDataHandlerCallbackType;

/**
* determines whether a new MessageReaderV2 and MessageWriterV2
* are allocated for each new roundtrip. 
*/
#define RDR_WRTR_ONESHOT 1

/**
* This class implements an http client that can send a request message and wait for a response.
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
* Handlers or callbacks
*
*/
class Client
{
public:
#pragma mark - constructors and destructors
    /**
     * Create a client that is not connected - however give it the info it needs
     * to try and establish a connection
     *
     * @param io - an io service
     * @param scheme of type HttpHeader::SchemeType
     * @param server - a string like google.com
     * @param port - a string like "443"
     */
    Client(boost::asio::io_service& io, HttpHeader::SchemeType scheme, std::string server, std::string port);
    
    /**
     * Create a client that is not connected - however give a url it needs
     * to try and establish a connection.
     * @param io - an io service
     * @param url - a string like "https://username:password@www.google.com/path1/path2:433?one=1111"
     */
    Client(boost::asio::io_service& io, std::string uri);

    /**
    * Create a client with an established connection. In this case a call to
    * connect will fail with an exception - as it is a logic error
    */
    Client(boost::asio::io_service& io, ConnectionInterface* conn);

    Client(const Client& other) = delete;
    Client& operator=(const Client&) = delete;
    
    ~Client();
    
#pragma mark - getters and setters
    
    MessageReaderV2SPtr  getResponse();
    
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
    * Writes the complete message to the connected host inlcuding body and trailers (if there are any).
    *
    * If not connected will perform a connect before transmitting the message. Will wait for a response
    * and call the onResponseHandler CB when the response is complete.
    *
    * If the request is to have a body it must be already attached to the MessageInterface.
    * A content_length header will be attached by the client.
    *
    * If the request is to have NO body use this method with the requestMessage.body == nullptr;
    */
    void asyncWrite(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    /**
    * The following are three ways to add a body to a request. The first two take a string or
    * a shared pointer to an MBuffer - these are contiguous buffers and can be sent with a single
    * asyncWrite
    */
    void asyncWrite(MessageBaseSPtr requestMessage,  std::string& body_str, ResponseHandlerCallbackType cb);
    void asyncWrite(MessageBaseSPtr requestMessage,  MBufferSPtr body_sptr, ResponseHandlerCallbackType cb);
    void asyncWrite(MessageBaseSPtr requestMessage,  BufferChainSPtr chain_sptr, ResponseHandlerCallbackType cb);
    /**
    * The FBuffer is fragmented and requires multiple asyncWrite operations to send it
    */
    void asyncWrite(MessageBaseSPtr requestMessage,  FBufferSharedPtr body, ResponseHandlerCallbackType cb);

    /**
    * Sends the first line and headers of the request message only
    * and expects any body data to be sent using asyncWriteBodyData
    * in which case the body will be "chunk" encoded and the headers
    * will be completed to indicate this.
    *
    * Dont use this method IF there is no body data, use asyncWrite
    */
    void asyncWriteHeaders(MessageBaseSPtr requestMessage, WriteHeadersCallbackType cb);
    
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
    void asyncWriteTrailers(MessageBaseSPtr requestMessage,  AsyncWriteCallbackType cb);
    
    /**
    * Called to signal end-of-message. This function will
    * (if necessary) add the chunk-encoding trailer.
    */
    void end();
    
#pragma mark - friend utility functions
   
    friend std::string traceClient(Client& client);
    friend std::string traceRequestMessage(MessageBase& request);

protected:
    void internalConnect();
    void internalWrite();

    void _async_write(MessageBaseSPtr requestMessage,  ResponseHandlerCallbackType cb);
    void putHeadersStuffInBuffer();
    void setupUrl(std::string url);
    void defaultHeaders();
    void setContentLength();
    
    std::string _url; // resource locator
    std::string _uri; // really path
    std::string _scheme;    // http or https
    std::string _server;  // this is without the port localhost for example
    std::string _host;           // as used in the headers eg localhost:9991 for example
    std::string _host_with_port; // as used in the headers eg localhost:9991 for example
    std::string _port;
    std::string _path;
    Url::Query  _query;
    std::string _queryStr;

    boost::asio::io_service&                        _io;
    MessageBaseSPtr                                 _current_request;
    MBufferSPtr                                     _body_mbuffer_sptr;
    FBufferSharedPtr                                _body_fbuffer_sptr;
    std::shared_ptr<MessageWriterV2>                  _wrtr;
    std::shared_ptr<MessageReaderV2>                  _rdr;
    
//    TCPConnection*                                  _conn_ptr;
    std::shared_ptr<TCPConnection>                  _conn_shared_ptr;
    ReadSocketInterface*                            _readSock;
    
    std::function<void(Marvin::ErrorType& err)>     _goCb;
    ResponseHandlerCallbackType                     _response_handler;
    ResponseHandlerCallbackType                     _on_headers_handler;
    ClientDataHandlerCallbackType                   _on_data_handler;
//    bool        _oneTripOnly;
//    
//    std::string _service;   //used by boost for resolve and connnect http/https or a port number
//    std::string _server;    // as used in boost resolve/connect WITHOUT port number
    
};
#endif

