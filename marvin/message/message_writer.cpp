//
//  message_writer.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "buffer.hpp"
#include "message_writer.hpp"
#include "marvin_error.hpp"
#include <exception>
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#define NO_CTORTRACE 1
#define NO_FDTRACE 1

std::string traceWriter(MessageWriter& writer)
{
    std::stringstream ss;
//    ss  << traceMessageV2(writer)
//        << "[" << writer._haveContent << "]"
//        << " body.len: " << writer._bodyContent.size() ;
//    std::stringstream ss;
//    ss << "Just testing";
    return ss.str();
}

//MessageWriter::MessageWriter(boost::asio::io_service& io, TCPConnection& conn):_io(io), _conn(conn), _m_header_buf(1000)
MessageWriter::MessageWriter(boost::asio::io_service& io, ISocketSPtr conn):_io(io), _conn(conn), _m_header_buf(1000)
{
    LogTorTrace();
}

#if 0
MessageWriter::MessageWriter(boost::asio::io_service& io, bool is_request):_io(io)
{
    LogTorTrace();
    _isRequest = is_request;
    // set default version
    setHttpVersMajor(1);
    setHttpVersMinor(1);

}
#endif
MessageWriter::~MessageWriter()
{
//    delete _m_header_buf;
    LogTorTrace();
}

void MessageWriter::putHeadersStuffInBuffer()
{
    MessageBaseSPtr msg = _currentMessage;
    _m_header_buf.empty();
    serializeHeaders(*msg, _m_header_buf);
    LogDebug("request size: ");
}
void MessageWriter::onWriteHeaders(Marvin::ErrorType& ec)
{
    // put the  first line and header into a buffer
//    IWriteSocket* wsock = getWriteSocket();
//    wsock.asyncWrite();
}
void MessageWriter::asyncWrite(MessageBaseSPtr msg, std::string& body_string, WriteMessageCallbackType cb)
{
   
    if(body_string.size() == 0 ) {
        asyncWrite(msg, cb);
    } else {
//        std:: cout << std::endl << "body buffer write: " << body_string << std::endl;
        _body_buffer_string = body_string;
        _body_buffer_chain_sptr = buffer_chain(body_string);
        asyncWrite(msg, cb);
    }
}
void MessageWriter::asyncWrite(MessageBaseSPtr msg, MBufferSPtr body_mb_sptr, WriteMessageCallbackType cb)
{
    assert(body_mb_sptr != nullptr);
    _body_buffer_chain_sptr = buffer_chain(body_mb_sptr);
    asyncWrite(msg, cb);
}

void MessageWriter::asyncWrite(MessageBaseSPtr msg, BufferChainSPtr body_chain_sptr, WriteMessageCallbackType cb)
{
    _body_buffer_chain_sptr = body_chain_sptr;
    asyncWrite(msg, cb);
}

void
MessageWriter::asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb)
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
MessageWriter::asyncWriteHeaders(MessageBaseSPtr msg,  WriteHeadersCallbackType cb)
{
    putHeadersStuffInBuffer();
    
    _conn->asyncWrite(_m_header_buf, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){

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
    if( ( ! _body_buffer_chain_sptr) || ( _body_buffer_chain_sptr->size() == 0) ) {
        LogWarn("writing empty body");
        Marvin::ErrorType ee = Marvin::make_error_ok();
        auto pf = std::bind(cb, ee);
        _io.post(pf);
    } else{
#if 0
        /**
        * PROBLEM - this copies the body - find a better way
        * @todo - there is a bug here when using buffer chains
        */
        std::cout << std::endl << __FUNCTION__ << " "<< _body_buffer_chain_sptr->to_string() << std::endl;
        /**
        * This is a hack to overcome some bug in BufferChain usage
        */
        _body_mbuffer_sptr = _body_buffer_chain_sptr->amalgamate();
        _conn->asyncWrite(*_body_mbuffer_sptr, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
#else
        _conn->asyncWrite(_body_buffer_chain_sptr, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
#endif
            LogDebug("");
            auto pf = std::bind(cb, ec);
            _io.post(pf);
        });
    }
    
}

void MessageWriter::asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb)
{
    auto bf = boost::asio::buffer(data);
    _conn->asyncWrite(bf, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriter::asyncWriteBodyData(MBuffer& data, WriteBodyDataCallbackType cb)
{
    _conn->asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}

//void MessageWriter::asyncWriteBodyData(FBuffer& data, WriteBodyDataCallbackType cb)
//{
//    _conn->asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
//        cb(err);
//    });
//}
void MessageWriter::asyncWriteBodyData(boost::asio::const_buffer data, WriteBodyDataCallbackType cb)
{
    _conn->asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}

void MessageWriter::asyncWriteTrailers(MessageBaseSPtr msg,  AsyncWriteCallbackType cb)
{
}

void MessageWriter::end()
{
}
