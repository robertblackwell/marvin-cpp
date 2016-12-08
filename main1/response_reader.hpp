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
#include "Message.hpp"
#include "Parser.hpp"
#include "RBLogger.hpp"

//
// Two call back types required by ResponseReader
//
// call back when reading headers
typedef std::function<void(Marvin::ErrorType& err, MessageInterface* msg)>          ResponseCbType;
// Callback when reading body data
typedef std::function<void(Marvin::ErrorType& err, FBuffer& fBuf)>                  AsyncReadBodyCallback;

/**
 * Instances of this class represent an incoming http(s) response message from a socket/stream.
 * Please note the "incoming" because the ResponseReader is seeking to provide a dynamic interface
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
 * The ResponseReader object calls on a Connection object to do the reading from the underlying socket/stream.
 * The ResponseReader provides buffers to the Connection class at the time the read is initiated
 * and receives an error code and bytes_transfered back from the Connection via the read handler.
 * This is pretty much standard Boost::asio behaviour
 *
 * Internally the ResponseReader has a buffer that is used for reading the initial part of the message
 * that contains the message headers. From these buffers the ResponseReaders fills in the various header
 * and first line fields and never has to pass that buffer up to the "user" components.
 *
 * Body data is buffered differently, there are two reasons for this:
 *  -   to promote an easy "streaming" style interface body buffers are passed off to the caller
 *      and then become the callers responsibility. This allows the ResponseReader to make decisions about
 *      the size of these buffers and to do a degree of "flowing" - to use a nodejs term.
 *
 *  -   secondly the body data is DE_CHUNKED. Which means that a block of incoming data that may be contiguous
 *      when read has holes in it after DE-CHUNKING (the chunk headers need to be missing). To avoid copying
 *      the chunk bodies into a contiguous buffer a concept of a Fragmented buffer has been introduced. This is
 *      what the ResponseReader passes up to the caller via the read callback functions.
 *  
 *      To facilitate good memory management the Fragmented Buffers are passed up/back via a std::move()
 *      call on a std::unique<FBuffer>
 *
 */
class ResponseReader : public Parser, public MessageBase
{
public:
    ResponseReader(Connection& conn, boost::asio::io_service& io);
    ~ResponseReader();
    //
    // Starts the reading process and invokes cb when all headers have been received
    // Passes back an error code to indicate success/failure. THe headers can be obtained from the
    // the ResponseReader object.
    // This method should only be called ONCE
    //
    void readResponse(ResponseCbType cb);
    
    //
    // These methods read the body data. Should be called multiple times until the end of message
    // body is signalled by the returned error code being equal to Narvin::make_error_eob()
    //
    // The callback to ReadBody receives an error code and a unique pointer to a fragmented buffer(FBuffer)
    // This is because the body data returned in that buffer is "de-chunked" and the buffer MAY contain
    // multiple chunk bodies. It was done this way to prevent copying the body data to eliminate the chunk headers.
    //
    //  The unique pointer to an FBuffer passes ownership of the buffer to the callback. A new buffer will be
    //  provided on each call to the callback
    //
    void readBody(AsyncReadBodyCallback cb);
    
    //
    void readBodyData(MBuffer& mb, AsyncReadCallback cb);

private:
    void makeBodyBuffer(std::size_t size);

    // not used as yet
    void readBodyHandler(MBuffer& mb);
    
    // returns the ResponseReader object as it is also a MessageInterface object
    MessageInterface* currentMessage();

    // Called by the parser when a lump of (de-chunked) body data is available. Generally it Splices
    // the lump into a Fragmented buffer
    void OnBodyData(void* buf, int len);
    
    // These method starts all reading operations
    void startRead();
    void startReadBody();
    
    // io.post() the callback functions
    void postResponseCallback(Marvin::ErrorType& er);
    void postBodyCallback(Marvin::ErrorType& er);

    // This method handles the result of all reading operations either headers or body
    void asyncReadHandler(Marvin::ErrorType& er, std::size_t bytes_transfered);
    
    // utility functions used by asyncReadHandler to do the details or error handling for
    // read results that have errors
    void handleError(Marvin::ErrorType& er);
    void handleParseError();
    
private:
    Connection&                 _conn;
    bool                        _headersComplete;
    bool                        _messageComplete;
    boost::asio::io_service&    _io;
    std::size_t                 _body_buffer_size;
    
    //read buffer
    MBuffer*        readBuffer;
    MBuffer*        bodyBuffer;
    
    // These 3 properties handle the situation where body data arrives while still processing the headers
    bool            _readBodyStarted;
    void*           _cachedBodyData;
    std::size_t     _cachedBodyDataLength;
    
    // These 2 properties provide access to the de-chunked data that is provided by the c parser
    void*           _dechunkedData;
    std::size_t     _dechunkedDataLength;
    
    // These two are used for buffering body data. Body data is ALWAYS stored inro _bodyMBuffer
    // and _bodyFBuffer (and _bodyFBufferUniquePtr) are used to keep track of the possibly multiple
    // chunk bodies that may be contained in the buffer arriving from a single read
    MBuffer*            _bodyMBuffer;
    FBuffer*            _bodyFBufferPtr;
    FBufferUniquePtr    _bodyFBufferUniquePtr;
    
//    AsyncReadCallback       _bodyCb;
    AsyncReadBodyCallback   _bodyCallback;
    ResponseCbType          _responseCb;
};

ResponseReader::ResponseReader(Connection& conn, boost::asio::io_service& io): _io(io), _conn(conn)
{
    _headersComplete = false;
    _messageComplete = false;
    _cachedBodyDataLength = 0;
    _cachedBodyData  = nullptr;
    _readBodyStarted = false;
    _dechunkedData   = nullptr;
    _dechunkedDataLength = 0;
    _body_buffer_size   = 10000;
    
}
ResponseReader::~ResponseReader()
{
    delete readBuffer;
}

//
// This function is the only place where each piece of de-chunked body data is seen.
// So this is where the "fragmentation" of the buffer has to take place.
// In addition if any body data accompanies the buffer that completed the headers
// this is the only place that sees that data.Hence it needs to be cached for returning on the
// next read of body data.
//
void ResponseReader::OnBodyData(void* buf, int len)
{
    //
    // PROBLEM - there is no guarentee that this will be called only once
    // during parsing of the buffer that completes the header. The solution
    // is to use an FBuffer for this operation as well.
    //
    // I would like to be able to guarentee that the memory pointed to by "buf"
    // would stay valid long enough to be used without memcpy'ing it
    //
    std::string tmp((char*)buf, len);
    LogDebug("tmp:", tmp);
    FLogDebug("buf:%x", (long)buf);
    FLogDebug("readBuffer: %x", readBuffer->data());
    FLogDebug("buf+len: %x",(long)((char*)buf+len));
    if( ! _readBodyStarted ){
        //
        // we got here while parsing a header buffer that simply had some body data in it.
        // so we MAY not have a _bodyFBufferUniquePtr available.
        // So make one and then add this data to it. Remember this may happen a number of times
        // so a single buffer will not work.
        //
        if( _bodyMBuffer == nullptr ){
            makeBodyBuffer(len);
        }
        _bodyFBufferUniquePtr->copyIn(buf, len);
    }else{
        // we are getting body data from a read that was explicitly initiated.
        // maybe we want to store it as what is passed in is "de-chunked" data
        
        //
        // Need to do something better here - add start and end pointers to the readBuffer
        //
        // Do we already have an FBuffer for body data ? Yes we MUST or the read that resulted in us coming
        // here could not have got started.
        //
        assert( (_bodyFBufferUniquePtr != nullptr) );
        _bodyFBufferUniquePtr->addFragment(buf, len);
    }
    LogDebug("exit buf:",  std::string((char*)buf, len));
};
//
// create an FBuffer big enough to hold size bytes and the corresponding FBufferUniquePtr
//
void ResponseReader::makeBodyBuffer(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBuffer = new MBuffer(len);
    std::unique_ptr<FBuffer> tmp(new FBuffer(_bodyMBuffer));
    _bodyFBufferUniquePtr = std::move(tmp);
}
MessageInterface* ResponseReader::currentMessage(){
    MessageInterface* m = this;
    return this;
}

void ResponseReader::readBody(AsyncReadBodyCallback cb)
{
    this->_bodyCallback = cb;
    Marvin::ErrorType er = ( isFinishedMessage() )? Marvin::make_error_eom() : Marvin::make_error_ok();
    
    if( ! _readBodyStarted ){
        // this is the first readBody
        _readBodyStarted = true; // signal an explicit readBodyData has been issued
        if( _bodyFBufferUniquePtr != nullptr ){
            // there is body data hanging around in _bodyFBuffer from the header reading , so return it
            postBodyCallback(er);
//            auto pf = std::bind(_bodyCallback, er, *_bodyFBufferUniquePtr);
//            _io.post(pf);
//            return;
        }else if( isFinishedMessage() ) {
            // there is no body data hanging around AND the messsage has been fully received
            // just return empty buffer and EOM
            makeBodyBuffer(_body_buffer_size);
            er = Marvin::make_error_eom();
            postBodyCallback(er);
//            auto pf = std::bind(_bodyCallback, er, *_bodyFBufferUniquePtr);
//            _io.post(pf);
//            return;
        } else{
            // no body data hanging around and NOT EOM - so start a read
            makeBodyBuffer(_body_buffer_size);
            startReadBody(); // need to do some body stuff so maybe startBodyRead()
        }
    }else{
        // not the first readBody
        if( isFinishedMessage() ){
            // there is no(SHOULD BE NO) body data hanging around AND the messsage has been fully received
            // just return empty buffer and EOM
            assert(false); // actually should not get here
            makeBodyBuffer(0);
            er = Marvin::make_error_eom();
            postBodyCallback(er);
//            auto pf = std::bind(_bodyCallback, er, *_bodyFBufferUniquePtr);
//            _io.post(pf);
            
        }else{
            // not first readBody AND not finished with the message - start another read
            makeBodyBuffer(_body_buffer_size);
            startReadBody(); // need to do some body stuff so maybe startBodyRead()
        }
    }
}

void ResponseReader::readBodyData(MBuffer& mb, AsyncReadCallback cb)
{
}
void ResponseReader::readBodyHandler(MBuffer& mb){
}

void ResponseReader::handleError(Marvin::ErrorType& er){
    
}
void ResponseReader::handleParseError(){
    
}

void ResponseReader::startReadBody()
{
    auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    MBuffer* mb = _bodyMBuffer;
    readBuffer = _bodyMBuffer;
    _conn.asyncRead(*readBuffer, h);
}

void ResponseReader::startRead(){
    auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    _conn.asyncRead(*readBuffer, h);
}

void ResponseReader::postBodyCallback(Marvin::ErrorType& er)
{
    auto pf = std::bind(_bodyCallback, er, *_bodyFBufferUniquePtr);
    _io.post(pf);
}
void ResponseReader::postResponseCallback(Marvin::ErrorType& er)
{
    MessageInterface* m = currentMessage();
    auto pf = std::bind(_responseCb, er, m);
    _io.post(pf);
}

void ResponseReader::asyncReadHandler(Marvin::ErrorType& er, std::size_t bytes_transfered){
    LogDebug("entry");
    //        if( er != Error::success() ){
    //            handleError(er);
    //            return;
    //        }
    readBuffer->setSize(bytes_transfered);
    MBuffer& mb = *readBuffer;
    bool saved_EOH = isFinishedHeaders();
    bool saved_EOM = isFinishedMessage();
    int nparsed;
    int sz = (int)mb.size();
    LogDebug("sz: ", sz);
    if( sz == 0 ){
        LogDebug("zero ");
        nparsed = sz;
        this->appendEOF();
    }else{
        nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    }
    bool ee = this->isError();
    
    if( ee ){
        LogDebug(" parse error ");
        handleParseError();
        return;
    }
    
    if( ! isFinishedHeaders() ){
        // finish the headers - keep reading
        startRead();
    }else if( isFinishedHeaders() && ! saved_EOH ){
        // headers are finishing with this block of read data
        Marvin::ErrorType er = Marvin::make_error_ok();
        MessageInterface* m = currentMessage();
        auto pf = std::bind(_responseCb, er, m);
        //
        // This buffer finished the headers.
        // See if any body data came in  and if so cache it
        //
        if( isFinishedMessage() ){
            //
            // We finisihed the headers and message with the same read.
            // If there was any body data the onBodyData callback has collected it into
            // a fragmented buffer for us. We have nothing to do.
            // All the body data will be returned on the first readBody call
            //
            LogDebug("got EOM as well as EOH");
            //
            // Do we want to signal EOM at this stage - no one the first readBody call
            //
            
//        }else if( _cachedBodyDataLength != 0 ){
//            // we have SOME of the the body but NOT ALL.
//            // Cache the body data
//            // and prepare for readBodyData calls
//            int ofset = (int)mb.size() - (int)_cachedBodyDataLength;
//            char* bdy = ((char*)mb.data()) + ofset;
//            LogDebug("not EOM but did get some body data");
        } else {
            LogDebug("EOH but not EOM ");
            //
            // we may have got some body data as well. If so it has been
            // captured by the onBodyData callback and stashed in a fragmented buffer.
            // It will be returned on the first readBody call.
            //
            // Do nothing
            //
        }
        _io.post(pf);
    }else if( ! saved_EOM ){
        //
        // We are now reading the body as EOH was already set when we entered this function
        // Determine whether its EOM and post callback

        LogDebug("reading body data - NOT EOM");
        Marvin::ErrorType er = ( isFinishedMessage() )? Marvin::make_error_eom() : Marvin::make_error_ok();
        auto pf = std::bind(_bodyCallback, er, *_bodyFBufferUniquePtr);
        _io.post(pf);
        
    }else{
        LogWarn("else - should not get here", sz);
    }
    LogDebug("exit");
    
}
void ResponseReader::readResponse(ResponseCbType cb)
{
    // need to set up a buffer
    readBuffer = new MBuffer(10000);
    int nparsed;
    this->_responseCb = cb;
    startRead();
}


#endif