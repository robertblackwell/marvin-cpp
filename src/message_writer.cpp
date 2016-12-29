//
//  message_writer.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include "message_writer.hpp"
#include "marvin_error.hpp"
#include <exception>
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

std::string traceWriter(MessageWriter& writer)
{
    std::stringstream ss;
    ss  << traceMessage(writer)
        << "[" << writer._haveContent << "]"
        << " body.len: " << writer._bodyContent.size() ;
//    std::stringstream ss;
//    ss << "Just testing";
    return ss.str();
}


MessageWriter::MessageWriter(boost::asio::io_service& io, bool is_request):_io(io)
{
    _isRequest = is_request;
    // set default version
    setHttpVersMajor(1);
    setHttpVersMinor(1);

}
MessageWriter::~MessageWriter(){ LogDebug("");}

void
MessageWriter::setContent(std::string& contentStr)
{
    _haveContent = true;
    _bodyContent = contentStr;
    setHeader("Content-length", std::to_string(contentStr.size()));
    LogDebug("");
}
std::string&
MessageWriter::getBody()
{
    if( ! _haveContent ) _bodyContent = "";
    return _bodyContent;
}
void MessageWriter::putHeadersStuffInBuffer()
{
    
    LogDebug("request size: ");
    std::ostream _headerStream(&_headerBuf);
    
    if( isRequest() ){
        std::string s = httpMethodString((HttpMethod) this->_method);
        _headerStream << s << " " << _uri << " HTTP/1.1\r\n";
    } else{
        _headerStream << "HTTP/1.1 " << _status_code << " " << _status <<  "\r\n";
    }
    
    for(auto const& h : _headers) {
        _headerStream << h.first << ": " << h.second << "\r\n";
    }
    // end of headers
    _headerStream << "\r\n";

    // debugging only
    boost::asio::streambuf::const_buffers_type bufs = _headerBuf.data();
    std::string ss(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + _headerBuf.size() );
    
    LogDebug("request text [", ss, "]");
    
}
void MessageWriter::onWriteHeaders(Marvin::ErrorType& ec)
{
    // put the  first line and header into a buffer
//    WriteSocketInterface* wsock = getWriteSocket();
//    wsock.asyncWrite();
}

void
MessageWriter::asyncWrite(WriteMessageCallbackType cb)
{
    asyncWriteHeaders([this, cb](Marvin::ErrorType& ec){
        LogDebug(" cb: ", (long) &cb);
        // doing a full write of the message
        if( ec ){
            LogDebug("", ec.value(), ec.category().name(), ec.category().message(ec.value()));
            cb(ec);
        } else {
            asyncWriteFullBody([this, cb](Marvin::ErrorType& ec2){
                LogDebug(" cb: ", (long) &cb);
                auto pf = std::bind(cb, ec2);
//                _io.post(pf);
                cb(ec2);
            });
        }
    });
}

void
MessageWriter::asyncWriteHeaders(WriteHeadersCallbackType cb)
{
    putHeadersStuffInBuffer();
//    boost::asio::streambuf::const_buffers_type bufs = _headerBuf.data();
//    std::string ss(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + _headerBuf.size() );
    _writeSock->asyncWriteStreamBuf(_headerBuf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
        LogDebug("");
        // need to check and do something about insufficient write
            auto pf = std::bind(cb, ec);
//            _io.post(pf);
        cb(ec);
    });
}

//
// writes the entire body - precondition - we have the entire body already via call to setContent()
//
void MessageWriter::asyncWriteFullBody(WriteMessageCallbackType cb)
{
    LogDebug(" cb: ", (long) &cb);
    // if body not set throw exception
    if( ! _haveContent ){
        LogWarn("writing empty body");
//        throw std::invalid_argument("asyncWriteFullBody:: no content");
    } else if( _bodyContent.size() == 0 ){
        Marvin::ErrorType ee = Marvin::make_error_ok();
        cb(ee);
//        auto pf = std::bind(cb, ee);
//        _io.post(pf);
    } else{
        //
        // PROBLEM - this copies the body - find a better way
        //
        std::ostream _bodyStream(&_bodyBuf);
        _bodyStream << _bodyContent;
        
        _writeSock->asyncWriteStreamBuf(_bodyBuf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
            LogDebug("");
            auto pf = std::bind(cb, ec);
            _io.post(pf);
        });
    }
    
}
void
MessageWriter::asyncWriteBodyData(WriteBodyDataCallbackType cb)
{
}

void
MessageWriter::asyncWriteTrailers(AsyncWriteCallbackType cb)
{
}

void
MessageWriter::end()
{
}
