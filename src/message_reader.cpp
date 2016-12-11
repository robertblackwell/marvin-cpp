
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

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "repeating_timer.hpp"
#include "mock_connection.hpp"
#include "message_reader.hpp"


MessageReader::MessageReader(Connection& conn, boost::asio::io_service& io): _io(io), _conn(conn)
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

//
// This function is the only place where each piece of de-chunked body data is seen.
// So this is where the "fragmentation" of the buffer has to take place.
// In addition if any body data accompanies the buffer that completed the headers
// this is the only place that sees that data.Hence it needs to be cached for returning on the
// next read of body data.
//
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
MessageInterface* MessageReader::currentMessage(){
    MessageInterface* m = this;
    return this;
}

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

void MessageReader::readBodyHandler(MBuffer& mb){
}

void MessageReader::handleReadError(Marvin::ErrorType& er){
    
}
void MessageReader::handleParseError(){
    enum http_errno en = Parser::getErrno();

}

void MessageReader::startReadBody()
{
    auto h = std::bind(&MessageReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    MBuffer* mb = _bodyMBufferPtr;
    
    // WARNING - this is a leak of readBuffer
    
    _readBuffer = _bodyMBufferPtr;
    _conn.asyncRead(*_readBuffer, h);
//    _conn.asyncRead(*_bodyMBufferPtr, h);
}

void MessageReader::startRead(){
    LogDebug("");
    auto h = std::bind(&MessageReader::asyncReadHandler, this, std::placeholders::_1, std::placeholders::_2);
    _conn.asyncRead(*_readBuffer, h);
}
//
// called ONLY if there is body data in a header buffer
// in this case CANNOT overight the readBuffer
//
void MessageReader::makeBodyBufferDuringHeaderProcessing(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
//    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}
//
// create an FBuffer big enough to hold size bytes
//
void MessageReader::makeBodyBuffer(std::size_t size)
{
    std::size_t len = (size > _body_buffer_size) ? size : _body_buffer_size ;
    _bodyMBufferPtr = new MBuffer(len);
    _readBuffer = _bodyMBufferPtr ;
    _bodyFBufferPtr = new FBuffer(_bodyMBufferPtr);
}
bool MessageReader::hasBodyData(){
    if( _bodyFBufferPtr != nullptr ){
        return true;
    }
    return false;
}
void MessageReader::clearBodyBuffer(){
//    _readBuffer = nullptr;
    _bodyFBufferPtr = nullptr;
    _bodyMBufferPtr = nullptr;
}
void MessageReader::postBodyCallback(Marvin::ErrorType& er)
{
    auto pf = std::bind(_bodyCallback, er, _bodyFBufferPtr);
    _io.post(pf);
//    clearBodyBuffer();
}
void MessageReader::postResponseCallback(Marvin::ErrorType& er)
{
    MessageInterface* m = currentMessage();
    auto pf = std::bind(_responseCb, er);
    _io.post(pf);
}
void MessageReader::postMessageCallback(Marvin::ErrorType& er)
{
    MessageInterface* m = currentMessage();
    auto pf = std::bind(_messageCb, er);
    _io.post(pf);
}

void MessageReader::asyncReadHandler(Marvin::ErrorType& er, std::size_t bytes_transfered){
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
    LogDebug("exit");
    
}
void MessageReader::readHeaders(ReadHeadersCallbackType cb)
{
    LogDebug("");
    // this is trashy - when do I dispose/delete the buffer
    // have the size as a variable of constant
    _readBuffer = new MBuffer(10000);
    this->_responseCb = cb;
    startRead();
}
#pragma mark - read full message functions
//--------------------------------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------------------------------
void MessageReader::onMessage(Marvin::ErrorType& ec)
{
    LogDebug("");
    postMessageCallback(ec);
}
void MessageReader::onBody(Marvin::ErrorType& er, FBuffer* fBufPtr)
{
    LogDebug(" entry");
    // are we done - if not hang another read
    auto bh = std::bind(&MessageReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
    bool done = (er == Marvin::make_error_eom());
    
    if( er != Marvin::make_error_ok() )
    if( done ){
        bodyStream << *fBufPtr;
        delete fBufPtr;
        Marvin::ErrorType ee = Marvin::make_error_ok();
        onMessage(ee);
//        std::string msgStr = rdr_->MessageBase::str() + bodyStream.str();
        body = bodyStream.str();
//        std::string expectedBody = tcObj.result_body();
//        
//        bool vb = tcObj.verifyBody(body);
//        std::map<std::string, std::string> hh = rdr_->MessageBase::getHeaders();
//        bool vh = tcObj.verifyHeaders(hh);
//        if( vb && vh ){
//            LogDebug("Test for test case", _tcIndex ,"succeeded");
//        }else{
//            LogDebug("Test for test case", _tcIndex ," FAILED");
//            if( ! vb )
//                LogDebug("Body failed ", body, expectedBody );
//            if( ! vh ){
//                LogDebug("Headers failed");
//
//            }
//        }
    }else{
        // do something with fBuf
        //
        // lets accumulate the FBuffer into a body
        //
//        std::string xx = bodyStream.str();
        bodyStream << *fBufPtr;
        delete fBufPtr;
        this->readBody(bh);
    }
    LogDebug("exit");
    
}

void MessageReader::onHeaders(Marvin::ErrorType& er){
    LogDebug("entry");
    this->dumpHeaders(std::cout);

    auto bh = std::bind(&MessageReader::onBody, this, std::placeholders::_1, std::placeholders::_2);
    this->readBody(bh);
    LogDebug("exit");
}

void MessageReader::readMessage(ReadMessageCallbackType cb)
{
    this->_messageCb = cb;
    auto h = std::bind(&MessageReader::onHeaders, this, std::placeholders::_1);
    this->readHeaders(h);

}

