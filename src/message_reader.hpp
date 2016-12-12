#ifndef request_reader_hpp
#define request_reader_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "buffer.hpp"
#include "message.hpp"
#include "parser.hpp"
#include "rb_logger.hpp"

#include "read_socket_interface.hpp"
#include "callback_typedefs.hpp"
//
// Two call back types required by MessageReader
//

/// call back when reading headers
//typedef std::function<void(Marvin::ErrorType& err)>                         ReadHeadersCallbackType;

/// Callback when reading body data
//typedef std::function<void(Marvin::ErrorType& err, FBuffer* fBufPtr)>       ReadBodyDataCallbackType;

/// Call back when reading full message is complete
//typedef std::function<void(Marvin::ErrorType& err)>                         ReadMessageCallbackType;

/**
 * Instances of this class represent an incoming http(s) response message from a socket/stream.
 * Please note the "incoming" because the MessageReader is seeking to provide a dynamic interface
 * so that other components can start interacting with the reader (and the message) once the headers
 * have arrived and POSSIBLY before the body has arrived.
 *
 * In addition there is an effort to provide a "streaming" interface to the body data so that if the message
 * is being passed on that pass-on can start before all the body has arrived. This is an effort to reduce latency
 * and reduce the size of the buffering that is required.
 *
 * @TODO - trying to workout how to manage the body
 * @TODO - want the response to message to BE this class so what to base
 * ResponseREader on MessageBase
 *
 * Buffer strategy
 * ===============
 *
 * The MessageReader object calls on a Connection object to do the reading from the underlying socket/stream.
 * The MessageReader provides buffers to the Connection class at the time the read is initiated
 * and receives an error code and bytes_transfered back from the Connection via the read handler.
 * This is pretty much standard Boost::asio behaviour
 *
 * Internally the MessageReader has a buffer that is used for reading the initial part of the message
 * that contains the message headers. From these buffers the MessageReaders fills in the various header
 * and first line fields and never has to pass that buffer up to the "user" components.
 *
 * Body data is buffered differently, there are two reasons for this:
 *  -   to promote an easy "streaming" style interface body buffers allocated by the ResponseREader 
 *      and are passed off to the caller
 *      and then become the callers responsibility. This allows the MessageReader to make decisions about
 *      the size of these buffers and to do a degree of "flowing" - to use a nodejs term.
 *
 *  -   secondly the body data is DE_CHUNKED. Which means that a block of incoming data that may be contiguous
 *      when read has holes in it after DE-CHUNKING process(the chunk headers need to be ignored). To avoid copying
 *      the chunk bodies into a contiguous buffer a concept of a Fragmented buffer has been introduced. This is
 *      what the MessageReader passes up to the caller via the read callback functions.
 *  
 */
class MessageReader : public Parser, public MessageBase
{
public:
    MessageReader(ReadSocketInterface* readSock, boost::asio::io_service& io);
    ~MessageReader();
    //
    // Starts the reading process and invokes cb when all headers have been received
    // Passes back an error code to indicate success/failure or maybe even EOM. The headers can be obtained from the
    // the MessageReader object.
    // This method should only be called ONCE
    //
    void readHeaders(ReadHeadersCallbackType cb);
    
    //
    //  This methods read the body data. Should be called multiple times until the end of message
    //  body is signalled by the returned error code being equal to Marvin::make_error_eob() or
    //  or Marvin::make_error_eom()
    //
    //  The callback to readBody receives an error code and a  pointer to a fragmented buffer(FBuffer)
    //  This is because the body data returned in that buffer is "de-chunked" and the buffer MAY contain
    //  multiple chunk bodies. It was done this way to prevent copying the body data to eliminate the chunk headers.
    //  the callback is responsible for handling the buffer and delete-ing it when appropriate
    //
    //  The pointer to an FBuffer passes ownership of the buffer to the callback. A new buffer will be
    //  provided on each call to the callback
    //
    void readBody(ReadBodyDataCallbackType cb);
    
    //
    // This method starts the read of a full message including the body of the message. Use of this method
    // requires the buffering of the full message body.
    //
    void readMessage(ReadMessageCallbackType cb);
    
//    //
//    // Must be called to prvide a socket before any reading can be done
//    //
//    void setReadSock(ReadSocketInterface* rSock);
    
    std::string& getBody();
    
private:
    //----------------------------------------------------------------------------------------------------
    // private methods
    //----------------------------------------------------------------------------------------------------

    // not used as yet
    void readBodyHandler(MBuffer& mb);
    
    // returns the MessageReader object as it is also a MessageInterface object
    MessageInterface* currentMessage();

    // Called by the parser when a lump of (de-chunked) body data is available. Generally it Splices
    // the lump into a Fragmented buffer
    void OnBodyData(void* buf, int len);
    
    // These method starts all reading operations
    void startRead();
    void startReadBody();
    
    
    /// handler methods for reading a full message
    void onHeaders(Marvin::ErrorType& er);
    void onBody(Marvin::ErrorType& er, FBuffer* fBufPtr);
    void onMessage(Marvin::ErrorType& er);
    
    // io.post() the callback functions - this group manage body buffers and posting body callback
    void postResponseCallback(Marvin::ErrorType& er);
    void postBodyCallback(Marvin::ErrorType& er);
    void postMessageCallback(Marvin::ErrorType& er);
    bool hasBodyData();
    void makeBodyBuffer(std::size_t size);
    void makeBodyBufferDuringHeaderProcessing(std::size_t size);
    void clearBodyBuffer();

    // This method handles the result of all reading operations either headers or body
    void asyncReadHandler(Marvin::ErrorType& er, std::size_t bytes_transfered);
    
    // utility functions used by asyncReadHandler to do the details or error handling for
    // read results that have errors
    void handleReadError(Marvin::ErrorType& er);
    void handleParseError();
    
    //----------------------------------------------------------------------------------------------------
    // private properties
    //----------------------------------------------------------------------------------------------------
    ReadSocketInterface*        _readSock;
    boost::asio::io_service&    _io;
    std::size_t                 _body_buffer_size;
    std::size_t                 _header_buffer_size;
    
    //read buffer for headers
    MBuffer*        _readBuffer;
    
    // records whether a readBody has already been issued
    bool            _readBodyStarted;
    
    // These are used for buffering body data. Body data is ALWAYS stored inro _bodyMBufferPtr
    // The _bodyFBufferPtr are used to keep track of the possibly multiple
    // chunk bodies that may be contained in the buffer arriving from a single read
    MBuffer*        _bodyMBufferPtr;
    FBuffer*        _bodyFBufferPtr;
    
    /// used for full message read - body collection
    std::string                 body;
    std::ostringstream          bodyStream;

    
    ReadBodyDataCallbackType   _bodyCallback;
    ReadHeadersCallbackType    _responseCb;
    ReadMessageCallbackType    _messageCb;
};

#endif