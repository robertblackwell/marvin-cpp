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

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

void MessageWriter::asyncGetWriteSocket(ConnectCallbackType connectCb)
{
    assert(false);
}


MessageWriter::MessageWriter(boost::asio::io_service& io, bool is_request):_io(io)
{
    _isRequest = is_request;
}
MessageWriter::~MessageWriter(){}

void
MessageWriter::setContent(std::string& contentStr)
{
    _haveContent = true;
    _bodyContent = contentStr;
    setHeader("Content-length", std::to_string(contentStr.size()));
    LogDebug("");
}
void MessageWriter::putHeadersStuffInBuffer()
{
    
    LogDebug("request size: ");
    std::ostream _headerStream(&_headerBuf);
//    _headerStream << "GET /utests/echo/index.php HTTP/1.1\r\nHost: whiteacorn\r\n\r\n";
//    return;
    
    if( isRequest() ){
        std::string s = httpMethodString((HttpMethod) this->_method);
        _headerStream << s << " " << _uri << " HTTP/1.1\r\n";
    } else{
        _headerStream << " HTTP/1.1 " << _status_code << " " << _status <<  "\r\n";
    }
    
//    typedef std::map<std::string, std::string>::iterator it_type;
//    for(it_type iterator = this->headers_.begin(); iterator != this->headers_.end(); iterator++) {
//        _header_stream << iterator->first << ": " << iterator->second << "\r\n";
//    }
    
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
        LogDebug("");
        // doing a full write of the message
        if( ec ){
            cb(ec);
        } else {
            asyncWriteFullBody([this, cb](Marvin::ErrorType& ec2){
                auto pf = std::bind(cb, ec2);
                _io.post(pf);
//                cb(ec2);
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
            _io.post(pf);
//        cb(ec);
    });
    return;
//    asyncGetWriteSocket([this, cb](Marvin::ErrorType& er, Connection* conn){
//        if( !er ){
//            this->_writeSock = conn;
//            this->_writeSock->asyncWrite(_headerBuf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
//                LogDebug("");
//                // need to check and do something about insufficient write
//                cb(ec);
//            });
//        }else{
//        }
//    });
}

//
// writes the entire body - precondition - we have the entire body already via call to setContent()
//
void MessageWriter::asyncWriteFullBody(WriteMessageCallbackType cb)
{
    // if body not set throw exception
    if( ! _haveContent ){
        throw std::invalid_argument("asyncWriteFullBody:: no content");
    } else if( _bodyContent.size() == 0 ){
        Marvin::ErrorType ee = Marvin::make_error_ok();
        auto pf = std::bind(cb, ee);
        _io.post(pf);

//        cb(ee);
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
//            cb(ec);
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
