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
typedef std::function<void(Marvin::ErrorType& err, FBuffer* fBufPtr)>                 AsyncReadBodyCallback;
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
 *  -   to promote an easy "streaming" style interface body buffers allocated by the ResponseREader 
 *      and are passed off to the caller
 *      and then become the callers responsibility. This allows the ResponseReader to make decisions about
 *      the size of these buffers and to do a degree of "flowing" - to use a nodejs term.
 *
 *  -   secondly the body data is DE_CHUNKED. Which means that a block of incoming data that may be contiguous
 *      when read has holes in it after DE-CHUNKING process(the chunk headers need to be ignored). To avoid copying
 *      the chunk bodies into a contiguous buffer a concept of a Fragmented buffer has been introduced. This is
 *      what the ResponseReader passes up to the caller via the read callback functions.
 *  
 */
class ResponseReader : public Parser, public MessageBase
{
public:
    ResponseReader(Connection& conn, boost::asio::io_service& io);
    ~ResponseReader();
    //
    // Starts the reading process and invokes cb when all headers have been received
    // Passes back an error code to indicate success/failure or maybe even EOM. The headers can be obtained from the
    // the ResponseReader object.
    // This method should only be called ONCE
    //
    void readResponse(ResponseCbType cb);
    
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
    void readBody(AsyncReadBodyCallback cb);
    
private:
    //----------------------------------------------------------------------------------------------------
    // private methods
    //----------------------------------------------------------------------------------------------------

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
    
    // io.post() the callback functions - this group manage body buffers and posting body callback
    void postResponseCallback(Marvin::ErrorType& er);
    void postBodyCallback(Marvin::ErrorType& er);
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
    Connection&                 _conn;
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
    
    AsyncReadBodyCallback   _bodyCallback;
    ResponseCbType          _responseCb;
};

ResponseReader::ResponseReader(Connection& conn, boost::asio::io_service& io): _io(io), _conn(conn)
{
    _body_buffer_size   = 10000;
    _header_buffer_size = 10000;
    clearBodyBuffer();
}
ResponseReader::~ResponseReader()
{
    // how to know what to get rid of
    // delete _readBuffer;
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
    FLogDebug("_readBuffer: %x", _readBuffer->data());
    FLogDebug("buf+len: %x",(long)((char*)buf+len));
    if( ! _readBodyStarted ){
        //
        // we got here while parsing a header buffer that simply had some body data in it.
        // so we MAY not have a _bodyFBufferPtr available.
        // So make one and then add this data to it. Remember this may happen a number of times
        // so a single buffer will not work.
        //
        if( _bodyMBufferPtr == nullptr ){
            makeBodyBufferDuringHeaderProcessing(len);
        }
        // paranoia
        assert((_bodyMBufferPtr != nullptr));
        assert((_bodyFBufferPtr != nullptr));
        
        _bodyFBufferPtr->copyIn(buf, len);
    
    }else{
        // we are getting body data from a read that was explicitly initiated.
        // maybe we want to store it as what is passed in is "de-chunked" data
        
        //
        // Need to do something better here - add start and end pointers to the _readBuffer
        //
        // Do we already have an FBuffer for body data ? Yes we MUST or the read that resulted in us coming
        // here could not have got started.
        //
        assert( (_bodyFBufferPtr != nullptr) );
        _bodyFBufferPtr->addFragment(buf, len);
    }
    LogDebug("exit buf:",  std::string((char*)buf, len));
};
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

        if( hasBodyData() ){
            // there is body data hanging around in _bodyFBuffer from the header reading , so return it
            postBodyCallback(er);
        }else if( isFinishedMessage() ) {
            // there is no body data hanging around AND the messsage has been fully received
            // just return empty buffer and EOM
            makeBodyBuffer(_body_buffer_size);
            er = Marvin::make_error_eom();
            postBodyCallback(er);
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
            
        }else{
            // not first readBody AND not finished with the message - start another read
            makeBodyBuffer(_body_buffer_size);
            startReadBody(); // need to do some body stuff so maybe startBodyRead()
        }
    }
}

void ResponseReader::readBodyHandler(MBuffer& mb){
}

void ResponseReader::handleReadError(Marvin::ErrorType& er){
    
}
void ResponseReader::handleParseError(){
    enum http_errno en = Parser::getErrno();

}

void ResponseReader::startReadBody()
{
    auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    MBuffer* mb = _bodyMBufferPtr;
    
    // WARNING - this is a leak of readBuffer
    
    _readBuffer = _bodyMBufferPtr;
    _conn.asyncRead(*_readBuffer, h);
//    _conn.asyncRead(*_bodyMBufferPtr, h);
}

void ResponseReader::startRead(){
    auto h = std::bind(&ResponseReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    _conn.asyncRead(*_readBuffer, h);
}
//
// called ONLY if there is body data in a header buffer
// in this case CANNOT overight the readBuffer
//
void ResponseReader::makeBodyBufferDuringHeaderProcessing(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
//    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}
//
// create an FBuffer big enough to hold size bytes
//
void ResponseReader::makeBodyBuffer(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}
bool ResponseReader::hasBodyData(){
    if( _bodyFBufferPtr != nullptr ){
        return true;
    }
    return false;
}
void ResponseReader::clearBodyBuffer(){
//    _readBuffer = nullptr;
    _bodyFBufferPtr = nullptr;
    _bodyMBufferPtr = nullptr;
}
void ResponseReader::postBodyCallback(Marvin::ErrorType& er)
{
    auto pf = std::bind(_bodyCallback, er, _bodyFBufferPtr);
    _io.post(pf);
//    clearBodyBuffer();
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
    _readBuffer->setSize(bytes_transfered);
    MBuffer& mb = *_readBuffer;
    bool saved_EOH = isFinishedHeaders();
    bool saved_EOM = isFinishedMessage();
    int nparsed;
    bool readError = !(
                        ( er == Marvin::make_error_ok()) ||
                        ( er == Marvin::make_error_eob())||
                        ( er == Marvin::make_error_eom())||
                        ( er == Marvin::make_error_eof())
                        );
    if( readError ){
        handleReadError(er);
    }
    
    int sz = (int)mb.size();
    LogDebug("sz: ", sz);
    if( sz == 0 ){
        LogDebug("zero ");
        nparsed = sz;
        this->appendEOF();
    }else{
        nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    }
    bool ee = Parser::isError();
    
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
        //
        // This buffer finished the headers.
        // See if any body data came in  and if so cache it
        //
        if( isFinishedMessage() ){
            //
            // We finished the headers and message with the same read.
            // If there was any body data the onBodyData callback has collected it into
            // a fragmented buffer for us. We have nothing to do.
            // All the body data will be returned on the first readBody call
            //
            LogDebug("got EOM as well as EOH");
            //
            // Do we want to signal EOM at this stage - no one the first readBody call
            //
            er = Marvin::make_error_eom();
            
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
        // this should be packaged in a function
        MessageInterface* m = currentMessage();
        auto pf = std::bind(_responseCb, er, m);
        delete _readBuffer; _readBuffer = nullptr;// finished with headers
        _io.post(pf);
    }else if( ! saved_EOM ){
        //
        // We are now reading the body as EOH was already set when we entered this function
        // Determine whether its EOM and post callback
        LogDebug("reading body data - NOT EOM");
        Marvin::ErrorType er = ( isFinishedMessage() )? Marvin::make_error_eom() : Marvin::make_error_ok();
        postBodyCallback(er);
        
    }else{
        LogWarn("else - should not get here", sz);
    }
    LogDebug("exit");
    
}
void ResponseReader::readResponse(ResponseCbType cb)
{
    // this is trashy - when do I dispose/delete the buffer
    // have the size as a variable of constant
    _readBuffer = new MBuffer(10000);
    this->_responseCb = cb;
    startRead();
}


#endif