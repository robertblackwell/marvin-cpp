
#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include "boost_stuff.hpp"
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "buffer.hpp"
#include "message.hpp"
#include "parser.hpp"
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

//#include "repeating_timer.hpp"
//#include "mock_rsockection.hpp"
#include "read_socket_interface.hpp"
#include "message_reader.hpp"

std::string traceReader(MessageReader& rdr)
{
    std::stringstream ss;
    ss  << traceMessage(rdr)
        << " body.len: " << rdr.body.size() ;
    return ss.str();
}

MessageReader::MessageReader(ReadSocketInterface* readSock, boost::asio::io_service& io): _io(io), _readSock(readSock)
{
    LogDebug("");
    _body_buffer_size   = 10000;
    _header_buffer_size = 10000;
    _readBodyStarted    = false;
    clearBodyBuffer();
}
MessageReader::~MessageReader()
{
    LogDebug("");
    // how to know what to get rid of
    // delete _readBuffer;
}
#pragma mark -  simple public getters

MessageInterface* MessageReader::currentMessage()
{
//    MessageInterface* m = this;
    return this;
}

/*!
* Returns the message body as a std::string
* @return std::string
* !!! we need something much better for body buffers
*/
std::string& MessageReader::getBody()
{
    return body;
}

/*!
* Returns true if the reader has accumulated any body data
* @return bool
*/
bool MessageReader::hasBodyData()
{
    if( _bodyFBufferPtr != nullptr ){
        return true;
    }
    return false;
}

#pragma mark - Parser virtual overrides
/*!
*
* Overrides a virtual method in Parser.hpp/cpp 
* 
* Called whenever the http_parser sees a piece of body data
*
* This function is the only place where each piece of de-chunked body data is seen.
* So this is where the "fragmentation" of the buffer has to take place.
* In addition if any body data accompanies the buffer that completed the headers
* this is the only place that sees that data. Hence it needs to be cached that body data
* so it can be returned on the next read of body data.
*/
void MessageReader::OnBodyData(void* buf, int len)
{
    //
    // PROBLEM - there is no guarentee that this will be called only once
    // during parsing of the buffer that completes the header. The solution
    // is to use an FBuffer for this operation as well.
    //
    // I would like to be able to guarentee that the memory pointed to by "buf"
    // would stay valid long enough to be used without memcpy'ing it
    //
//    std::string tmp((char*)buf, len);
//    LogDebug("tmp:", tmp);
//    FLogDebug("buf:%x", (long)buf);
//    FLogDebug("_readBuffer: %x", _readBuffer->data());
//    FLogDebug("buf+len: %x",(long)((char*)buf+len));
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
    LogDebug("exit buf:");//,  std::string((char*)buf, len));
};
#pragma mark - empty methods, need to refactor
/// !!! do something with this
void MessageReader::readBodyHandler(MBuffer& mb)
{
}
/// !!! do something with this
void MessageReader::handleReadError(Marvin::ErrorType& er){
    LogDebug(
    "fd: ", _readSock->nativeSocketFD(),
    "error.value :", er.value(),
    "error.cat:", er.category().name(),
    "error.message", er.category().message(er.value()));

    
}
/// !!! do something with this
void MessageReader::handleParseError()
{
//    enum http_errno en = Parser::getErrno();
    LogDebug("fd: ", _readSock->nativeSocketFD() );
}

#pragma mark - buffer handling methods
/*!
* Internal method, called ONLY if there is body data in a header buffer that finished 
* the headers. We cache the body data.
* NOTE in this case CANNOT overight the readBuffer
*/
void MessageReader::makeBodyBufferDuringHeaderProcessing(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
//    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}

/*!
* create an FBuffer big enough to hold size bytes
*/
void MessageReader::makeBodyBuffer(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}
/*!
* clears pointers to various body buffers
* !!! check that we dnt leak
*/
void MessageReader::clearBodyBuffer()
{
//    _readBuffer = nullptr;
    _bodyFBufferPtr = nullptr;
    _bodyMBufferPtr = nullptr;
}
#pragma mark - public read methods
/**
* An interface method that is called to initiate the read of an entire
* message
*/
void MessageReader::readMessage(ReadMessageCallbackType cb)
{
    this->_messageCb = cb;
    auto h = std::bind(&MessageReader::onHeaders, this, std::placeholders::_1);
    this->readHeaders(h);
}

/*!
* An interface method called to initiate the reading of message first line and headers
* Completion signalled through the callback
*/
void MessageReader::readHeaders(ReadHeadersCallbackType cb)
{
    LogDebug("");
    // this is trashy - when do I dispose/delete the buffer
    // have the size as a variable of constant
    _readBuffer = new MBuffer(_header_buffer_size);
    this->_responseCb = cb;
    startRead();
}
/*!
* An interface method that is called to initiate an async read of some body data.
* Result is returned via the callback
*/
void MessageReader::readBody(ReadBodyDataCallbackType cb)
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
#pragma mark - internal methods to initiate reads
/*!
* An internal method that actually kicks off an async socket read to get some
* body data. The result of the read is handled by asyncReadHandler
*/
void MessageReader::startReadBody()
{
    auto h = std::bind(&MessageReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
//    MBuffer* mb = _bodyMBufferPtr;
    
    ///!!!! WARNING - this is a leak of readBuffer
    
    _readBuffer = _bodyMBufferPtr;
    _readSock->asyncRead(*_readBuffer, h);
}
/*!
* Internal method that actually kicks off an async read to get some of the message header
*/
void MessageReader::startRead()
{
    LogDebug(" fd: ", _readSock->nativeSocketFD());

    auto h = std::bind(&MessageReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    _readSock->asyncRead(*_readBuffer, h);
}
#pragma mark - the main internal method that is the socket read callback
/*!
* The main read handler. Handles reading data for headers and body data.
* Also has to be able to handle getting zero bytes and errors.
*
*/
void MessageReader::asyncReadHandler(Marvin::ErrorType& er, std::size_t bytes_transfered){
    LogDebug("entry fd: ", _readSock->nativeSocketFD());
    _readBuffer->setSize(bytes_transfered);
    MBuffer& mb = *_readBuffer;
    bool saved_EOH = isFinishedHeaders();
    bool saved_EOM = isFinishedMessage();
    int nparsed;
    
    int sz = (int)mb.size();
    LogDebug("sz: ", sz);
    if( sz == 0 ){
        LogInfo("", Marvin::make_error_description(er));

        nparsed = sz;
        this->appendEOF();
        // did the EOF finish a message - if so bubble it up else - error
        if( isFinishedMessage() ){
            Marvin::ErrorType m_er = Marvin::make_error_ok();
            postMessageCallback(m_er);
            LogDebug("exit fd: ", _readSock->nativeSocketFD() );
            return;
        }else{
            postMessageCallback(er);
            LogInfo("exit fd: ", _readSock->nativeSocketFD() );
            return;
        }
    }else{
        nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    }
    bool ee = Parser::isError();
    
    if( ee ){
        LogDebug(" parse error ");
        handleParseError();
        LogDebug("exit fd: ", _readSock->nativeSocketFD() );
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
        auto pf = std::bind(_responseCb, er);
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
    LogDebug("exit fd: ", _readSock->nativeSocketFD() );
    
}


#pragma mark - event methods
/*!
* called when a message is complete or when an error terminated reading/parsing
*/
void MessageReader::onMessage(Marvin::ErrorType& ec)
{
    LogDebug("");
    postMessageCallback(ec);
}

/*!
* called when all headers have been read or when reading headers is terminated by an error
*/
void MessageReader::onHeaders(Marvin::ErrorType& er)
{
    LogDebug("entry");
//    this->dumpHeaders(std::cout);

    auto bh = std::bind(&MessageReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
    this->readBody(bh);
    LogDebug("exit");
}
/*!
* called when a blob of body data has been read or when reading the body 
* has been terminated by an error
*/
void MessageReader::onBody(Marvin::ErrorType& er, FBuffer* fBufPtr)
{
    LogDebug(" entry");
    // are we done - if not hang another read
    auto bh = std::bind(&MessageReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
    bool done = ( (er == Marvin::make_error_eom()) || (er == Marvin::make_error_eof()) );
    
    if( done ){
        bodyStream << *fBufPtr;
        delete fBufPtr;
        Marvin::ErrorType ee = Marvin::make_error_ok();
        body = bodyStream.str();
//        LogTrace("onBody ", traceReader(*this));
        onMessage(ee);
    }else{
        // do something with fBuf
        //
        // lets accumulate the FBuffer into a body
        //
//        std::string xx = bodyStream.str();
        bodyStream << *fBufPtr;
//        body = bodyStream.str();
        delete fBufPtr;
        this->readBody(bh);
    }
    LogDebug("exit");
    
}
#pragma mark - post method for scheduling a callback to run later on the runloop

void MessageReader::postBodyCallback(Marvin::ErrorType& er)
{
    auto pf = std::bind(_bodyCallback, er, _bodyFBufferPtr);
    _io.post(pf);
}

/*!
* This should be called postHeaderCallback
*/
void MessageReader::postResponseCallback(Marvin::ErrorType& er)
{
//    MessageInterface* m = currentMessage();
    auto pf = std::bind(_responseCb, er);
    _io.post(pf);
}

void MessageReader::postMessageCallback(Marvin::ErrorType& er)
{
    auto pf = std::bind(_messageCb, er);
    _io.post(pf);
}


