//
//  message_writer.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "buffer.hpp"
#include "message_writer_v2.hpp"
#include "marvin_error.hpp"
#include <exception>
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#define NO_CTORTRACE 1
#define NO_FDTRACE 1

std::string traceWriterV2(MessageWriterV2& writer)
{
    std::stringstream ss;
//    ss  << traceMessageV2(writer)
//        << "[" << writer._haveContent << "]"
//        << " body.len: " << writer._bodyContent.size() ;
//    std::stringstream ss;
//    ss << "Just testing";
    return ss.str();
}

MessageWriterV2::MessageWriterV2(boost::asio::io_service& io, TCPConnection& conn):_io(io), _conn(conn), _m_header_buf(1000)
{
    LogTorTrace();
//    _isRequest = is_request;
    // set default version
//    setHttpVersMajor(1);
//    setHttpVersMinor(1);
//    _m_header_buf = new MBuffer(1000);

}
#if 0
MessageWriterV2::MessageWriterV2(boost::asio::io_service& io, bool is_request):_io(io)
{
    LogTorTrace();
    _isRequest = is_request;
    // set default version
    setHttpVersMajor(1);
    setHttpVersMinor(1);

}
#endif
MessageWriterV2::~MessageWriterV2()
{
//    delete _m_header_buf;
    LogTorTrace();
}

void MessageWriterV2::putHeadersStuffInBuffer()
{
    MessageBaseSPtr msg = _currentMessage;
    _m_header_buf.empty();
    serializeHeaders(*msg, _m_header_buf);
    LogDebug("request size: ");
}
void MessageWriterV2::onWriteHeaders(Marvin::ErrorType& ec)
{
    // put the  first line and header into a buffer
//    WriteSocketInterface* wsock = getWriteSocket();
//    wsock.asyncWrite();
}

void
MessageWriterV2::asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb)
{
    LogDebug("");
    _currentMessage = msg;
    asyncWriteHeaders(msg, [this, cb](Marvin::ErrorType& ec){
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
MessageWriterV2::asyncWriteHeaders(MessageBaseSPtr msg,  WriteHeadersCallbackType cb)
{
    putHeadersStuffInBuffer();
//    boost::asio::streambuf::const_buffers_type bufs = _headerBuf.data();
//    std::string ss(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + _headerBuf.size() );
//    std::string h = "GET / HTTP/1.1\r\nHost : whiteacorn.com\r\n\r\n";
//    const char* buf = h.c_str();
//    MBuffer* mb = new MBuffer(h.size()+10);
//    mb->append((void*)buf, h.size());
    
    _conn.asyncWrite(_m_header_buf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){

//    _conn.asyncWriteStreamBuf(_headerBuf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
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
void MessageWriterV2::asyncWriteFullBody(WriteMessageCallbackType cb)
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
        
        _conn.asyncWriteStreamBuf(_bodyBuf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
            LogDebug("");
            auto pf = std::bind(cb, ec);
            _io.post(pf);
        });
    }
    
}
void MessageWriterV2::asyncWriteBodyData(void* data, WriteBodyDataCallbackType cb)
{
}
void MessageWriterV2::asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb)
{
    auto bf = boost::asio::buffer(data);
    _conn.asyncWrite(bf, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriterV2::asyncWriteBodyData(MBuffer& data, WriteBodyDataCallbackType cb)
{
    _conn.asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriterV2::asyncWriteBodyData(FBuffer& data, WriteBodyDataCallbackType cb)
{
    _conn.asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriterV2::asyncWriteBodyData(boost::asio::const_buffer data, WriteBodyDataCallbackType cb)
{
    _conn.asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}

void MessageWriterV2::asyncWriteTrailers(MessageBaseSPtr msg,  AsyncWriteCallbackType cb)
{
}

void MessageWriterV2::end()
{
}
