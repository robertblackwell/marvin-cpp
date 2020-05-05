#ifndef marvin_message_reader_v2_hpp
#define marvin_message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

namespace Marvin {

class MessageReader;
typedef std::shared_ptr<MessageReader> MessageReaderSPtr;
typedef std::unique_ptr<MessageReader> MessageReaderUPtr;

/**
* \ingroup http
* \brief Knows how to read http messages in the form of MessageBase instances; note this class
*  is derived from MessageBase so the message read is bound to this reader as a subclass instance.
*
* ## Modes of use
*
* two modes of operation are provided:
*   -   read an entire messages in on method call including any message content or body and only notify the
*       caller when an entire message  is available.
*   -   read the header of an http messages in one method call and notify the caller of that, then have the caller
*       call again (anotehr method) to get chunks of the message bidy.
*
* \warning - this later mode is not yet implemented.
*
* \warning - instance of this class are one-time-only as after reading a message the reader instance actually
* holds the message data as this class is derived from MessageBase.
*
* Hence to read a second message a new instance of thei class is required.
*
* \note current implementation binds the message to the reader so a reader
* can only read one message and then needs to be discarded
*
 * Instances of this class represent an incoming http(s) response message from a socket/stream.
 * Please note the "incoming" because the MessageReader is seeking to provide a dynamic interface
 * so that other components can start interacting with the reader (and the message) once the headers
 * have arrived and POSSIBLY before the body has arrived.
 *
 * ### Buffer strategy
 *
 * The MessageReader object calls on a Connection object to do the reading from the underlying socket/stream.
 * The MessageReader provides buffers to the Connection class at the time the read is initiated
 * and receives an error code and bytes_transfered back from the Connection via the read handlers.
 * This is pretty much standard Boost::asio behaviour
 *
 * Internally the MessageReader has a buffer (MBufferSPtr) that is used for reading the initial part of the message
 * that contains the message headers. From this buffer the MessageReaders fills in the various header
 * and first line fields and never has to pass that buffer up to the "user" components.
 *
 * Body data is buffered differently, there are reasons for this:
 *  -   to implementpromote a"streaming" interface where the next available chunk of body data is return
 *      through a readBody() call.
 *  -   secondly the body data is DE_CHUNKED. Which means that a block of incoming data that may be contiguous
 *      when read, may have holes in it after the DE-CHUNKING process(the chunk headers need to be ignored). To limit copying
 *      the concept of a BufferChain was introduced. Each data chunk is stored a contiguous
 *      buffer and a collection of such buffers are stored non-contiguously in a BufferChain(
 *      (shared pointer to each buffer being retained in a std::vector). This is the structure that
 *      is returned to the called for readMessage() and readBody(). These buffers can easily be used a boost buffer
 *      sequence for i/o.
 *
 */
class MessageReader : public Marvin::MessageBase
{
public:

    using ReadMessageCallback = std::function<void(Marvin::ErrorType err)>;
    using ReadBodyCallback = std::function<void(Marvin::ErrorType err, Marvin::BufferChainSPtr chunkSPtr)>;

    static void configSet_HeaderBufferSize(long bsize);
    static void configSet_BodyBufferSize(long bsize);

    MessageReader( boost::asio::io_service& io, ISocketSPtr readSock);
    ~MessageReader();
    /*!
    *
    * NOTE : current implementation binds the message to the reader so a reader
    * can only read one message and then needs to be discarded
    *
    * Starts the reading process and invokes cb when all headers have been received
    * Passes back an error code to indicate success/failure or maybe even EOM. 
    * The headers can be obtained from the the MessageReader object as it
    * is an instance of MessageBase.
    *
    * This method should only be called ONCE
    */
    void readHeaders(std::function<void(Marvin::ErrorType err)> cb);
    
    /*!
    *  This methods read the body data. Should be called multiple times until the end of message
    *  body is signalled by the returned error code being equal to Marvin::make_error_eob() or
    *  or Marvin::make_error_eom()
    *
    *  The callback to readBody receives an error code and an instance (by value) of a BufferChain
    *  This is because the body data returned in that buffer is "de-chunked" and the buffer MAY contain
    *  multiple chunk bodies. 
    *  The callback is responsible for handling the buffer life time.
    *
    *  The BufferChain will release all the embedded buffers when the value goes out of scope.
    */
    void readBody(std::function<void(Marvin::ErrorType err, Marvin::BufferChainSPtr chunkSPtr)> bodyCb);
    
    /*!
    * This method starts the read of a full message including the body of the message. Use of this method
    * requires the buffering of the full message body.
    *
    * The message prefix (first line and headers) for the message can be obtained by remebering that
    * a MessageReader is an instance of MessageBase.
    *
    * The (de-chunked) body data can be obtained (as a BufferChain) by calling get_body_chain.
    *
    * The raw not-de-chunked body data can be obtained (as a BufferChain) by calling get_raw_body_chain
    */
//    void readMessage(std::function<void(Marvin::ErrorType err)> cb);
    void readMessage(ReadMessageCallback cb);

    /*!
    * Gets the message body as a std::string 
    * !!! need to do better
    */
    
//    Marvin::BufferChain  get_body_chain();
//    Marvin::BufferChain  get_raw_body_chain();
    
    friend std::string traceReader(MessageReader& rdr);
    
protected:
    //----------------------------------------------------------------------------------------------------
    // protected methods
    //----------------------------------------------------------------------------------------------------
    void p_read_headers(std::function<void(Marvin::ErrorType err)> cb);
    void p_read_message(std::function<void(Marvin::ErrorType err)> cb);
    void p_read_some_headers();
    void p_handle_header_read(Marvin::ErrorType er, std::size_t bytes_transfered);
    
    void p_read_body(std::function<void(Marvin::ErrorType err)> cb);
    void p_read_all_body();
    void p_read_some_body();
    void p_handle_body_read(Marvin::ErrorType er, std::size_t bytes_transfered);
    
    void p_read_body_chunk();
    void p_handle_body_chunk(Marvin::ErrorType er, std::size_t bytes_transfered);
    

    void p_make_new_body_buffer();
    void p_make_new_body_buffer_chain();
    void p_post_message_cb(Marvin::ErrorType er);
    void p_post_body_chunk_cb(Marvin::ErrorType er, Marvin::BufferChainSPtr chainSPtr);
    bool p_parser_ok(int nparsed, Marvin::MBuffer& mb);

    /**
    *  These methods are override for virtual methods in Parser
    */
    Marvin::MessageInterface* currentMessage();
    // void OnHeadersComplete(Marvin::MessageInterface* msg, void* body_start_ptr, std::size_t remainder);
    void OnHeadersComplete(Marvin::MessageInterface* msg);

    void OnMessageComplete(Marvin::MessageInterface* msg);
    void OnBodyData(void* buf, int len);
    void OnChunkBegin(int chunkLength);
    void OnChunkData(void* buf, int len);
    void OnChunkEnd();

    //----------------------------------------------------------------------------------------------------
    // protected properties 
    //----------------------------------------------------------------------------------------------------
    // static for config
    static std::size_t     s_headerBufferSize;
    static std::size_t     s_bodyBufferSize;
    
    // socket and io_service
    ISocketSPtr                  m_read_sock;
    boost::asio::io_service&     m_io;

    bool                         m_reading_full_message;
    bool                         m_reading_body;
    Parser::SPtr                 m_parser_sptr;
//    std::function<void(Marvin::ErrorType err)> m_read_message_cb;
    ReadMessageCallback m_read_message_cb;

    ReadBodyCallback                m_read_body_cb;
    
    /// @TODO - there is redundancy here needs to be cleaned up
    int                             m_total_bytes_read;
    Marvin::MBufferSPtr             m_header_buffer_sptr;
    Marvin::MBufferSPtr             m_body_buffer_sptr;

    Marvin::BufferChainSPtr         m_raw_body_buffer_chain_sptr;
    Marvin::BufferChainSPtr         m_body_buffer_chain_sptr;

    std::size_t                     m_body_buffer_size;
    std::size_t                     m_header_buffer_size;
    
    /// used to collect body data when doing a full message read
    std::string                     m_body;
};
} // namespcae
#endif
