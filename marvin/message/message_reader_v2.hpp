#ifndef message_reader_v2_hpp
#define message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "bufferV2.hpp"
#include "message.hpp"
#include "parser.hpp"
#include "rb_logger.hpp"

#include "read_socket_interface.hpp"
#include "callback_typedefs.hpp"

/**
    *
    * NOTE : current implementation binds the message to the reader so a reader
    * can only read one message and then needs to be discarded
    *
 * Instances of this class represent an incoming http(s) response message from a socket/stream.
 * Please note the "incoming" because the MessageReader is seeking to provide a dynamic interface
 * so that other components can start interacting with the reader (and the message) once the headers
 * have arrived and POSSIBLY before the body has arrived.
 *
 * Buffer strategy
 * ===============
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
class MessageReaderV2;
typedef std::shared_ptr<MessageReaderV2> MessageReaderV2SPtr;
typedef std::unique_ptr<MessageReaderV2> MessageReaderV2UPtr;

class MessageReaderV2 : public Parser, public MessageBase
{
public:

    static void configSet_HeaderBufferSize(long bsize);
    static void configSet_BodyBufferSize(long bsize);

    MessageReaderV2( boost::asio::io_service& io, ReadSocketInterfaceSPtr readSock);
    ~MessageReaderV2();
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
    *  multiple chunk bodies. It was done this way to prevent copying the 
    *  body data to eliminate the chunk headers.
    *  the callback is responsible for handling the buffer and delete-ing it when appropriate
    *
    *  The BufferChain will release all the embedded buffers when the value goes out of scope.
    */
    void readBody(std::function<void(Marvin::ErrorType err, BufferChain chunk)>);
    
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
    void readMessage(std::function<void(Marvin::ErrorType err)> cb);
    
    /*!
    * Gets the message body as a std::string 
    * !!! need to do better
    */
    
    BufferChain  get_body_chain();
    BufferChain  get_raw_body_chain();
    
    friend std::string traceReader(MessageReaderV2& rdr);
    
protected:
    static std::size_t     __headerBufferSize;
    static std::size_t     __bodyBufferSize;
    //----------------------------------------------------------------------------------------------------
    // private methods
    //----------------------------------------------------------------------------------------------------
    void read_headers(std::function<void(Marvin::ErrorType err)> cb);
    void read_message(std::function<void(Marvin::ErrorType err)> cb);
    void _read_some_headers();
    void _handle_header_read(Marvin::ErrorType er, std::size_t bytes_transfered);
    
    bool _reading_full_message;
    bool _reading_body;
    std::function<void(Marvin::ErrorType err)> _read_message_cb;
    std::function<void(Marvin::ErrorType err, BufferChain chunk)> _read_body_cb;
    
    void read_body(std::function<void(Marvin::ErrorType err)> cb);
    void _read_all_body();
    void _read_some_body();
    void _handle_body_read(Marvin::ErrorType er, std::size_t bytes_transfered);
    
    void _read_body_chunk();
    void _handle_body_chunk(Marvin::ErrorType er, std::size_t bytes_transfered);
    
    // buffer management
    MBufferSPtr _header_buffer_sptr;
    MBufferSPtr                     _body_buffer_sptr;
    FBufferSharedPtr                _body_fragments_sptr;

    BufferChain                     _raw_body_buffer_chain;
    BufferChain                     _body_buffer_chain;
    std::vector<FBufferSharedPtr>   _body_fragments_chain;

    void _make_new_body_buffer();
    void post_message_cb(Marvin::ErrorType er);
    void post_body_chunk_cb(Marvin::ErrorType er, BufferChain chain);
    bool parser_ok(int nparsed, MBuffer& mb);

    /**
    *  These methods are overridesa for virtual methods in Paser 
    */
    MessageInterface* currentMessage();
    void OnHeadersComplete(MessageInterface* msg, void* body_start_ptr, std::size_t remainder);
    void OnMessageComplete(MessageInterface* msg);
    void OnBodyData(void* buf, int len);
    void OnChunkBegin(int chunkLength);
    void OnChunkData(void* buf, int len);
    void OnChunkEnd();

    //----------------------------------------------------------------------------------------------------
    // private properties and methods
    //----------------------------------------------------------------------------------------------------
    ReadSocketInterfaceSPtr     _readSock;
    boost::asio::io_service&    _io;
    std::size_t                 _body_buffer_size;
    std::size_t                 _header_buffer_size;
    
    
    // records whether a readBody has already been issued
    bool            _readBodyStarted;
    
    // These are used for buffering body data. Body data is ALWAYS stored inro _bodyMBufferPtr
    // The _bodyFBufferPtr are used to keep track of the possibly multiple
    // chunk bodies that may be contained in the buffer arriving from a single read
    MBuffer*        _bodyMBufferPtr;
    FBuffer*        _bodyFBufferPtr;
    //read buffer for headers
    MBuffer*        _readBuffer;

    /// used for full message read - body collection
    std::string                 body;
    std::ostringstream          bodyStream;

    
    ReadBodyDataCallbackType   _bodyCallback;
    ReadHeadersCallbackType    _responseCb;
    ReadMessageCallbackType    _messageCb;
};

#endif
