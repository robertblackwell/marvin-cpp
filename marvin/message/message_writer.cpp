#include <marvin/message/message_writer.hpp>

#include <marvin/buffer/buffer.hpp>
#include <marvin/error/marvin_error.hpp>
#include <exception>
#include <marvin/external_src/trog/trog.hpp>

namespace Marvin {

Trog_SETLEVEL(LOG_LEVEL_WARN)

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

MessageWriter::MessageWriter(boost::asio::io_service& io, ISocketSPtr write_sock):m_io(io), m_write_sock(write_sock)
{
    LogTorTrace();
    m_current_message = nullptr;
    m_header_buf_sptr = nullptr;
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

#pragma mark - public methods
void MessageWriter::asyncWrite(MessageBaseSPtr msg, std::string& body_string, WriteMessageCallbackType cb)
{
   
    if(body_string.size() == 0 ) {
        asyncWrite(msg, cb);
    } else {
//        std:: cout << std::endl << "body buffer write: " << body_string << std::endl;
        m_body_buffer_string = body_string;
        m_body_buffer_chain_sptr = Marvin::BufferChain::makeSPtr(body_string);
        asyncWrite(msg, cb);
    }
}
void MessageWriter::asyncWrite(MessageBaseSPtr msg, Marvin::MBufferSPtr body_mb_sptr, WriteMessageCallbackType cb)
{
    assert(body_mb_sptr != nullptr);
    m_body_buffer_chain_sptr = Marvin::BufferChain::makeSPtr(body_mb_sptr);
    asyncWrite(msg, cb);
}

void MessageWriter::asyncWrite(MessageBaseSPtr msg, Marvin::BufferChainSPtr body_chain_sptr, WriteMessageCallbackType cb)
{
    m_body_buffer_chain_sptr = body_chain_sptr;
    asyncWrite(msg, cb);
}

void
MessageWriter::asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb)
{
    LogDebug("");
    MessageBaseSPtr tmp = msg;
    m_current_message = msg;
    asyncWriteHeaders(msg, [this, cb](Marvin::ErrorType& ec){
        LogDebug(" cb: ", (long) &cb);
        // doing a full write of the message
        if( ec ){
            LogDebug("", ec.value(), ec.category().name(), ec.category().message(ec.value()));
            cb(ec);
        } else {
            p_async_write_full_body([this, cb](Marvin::ErrorType& ec2){
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
    p_put_headers_stuff_in_buffer();
    
    m_write_sock->asyncWrite(*m_header_buf_sptr, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){

        LogDebug("");
        // TODO need to check and do something about insufficient write
            auto pf = std::bind(cb, ec);
//            _io.post(pf);
        cb(ec);
    });
}


void MessageWriter::asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb)
{
    auto bf = boost::asio::buffer(data);
    m_write_sock->asyncWrite(bf, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriter::asyncWriteBodyData(Marvin::MBuffer& data, WriteBodyDataCallbackType cb)
{
    m_write_sock->asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
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
    m_write_sock->asyncWrite(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}

void MessageWriter::asyncWriteTrailers(MessageBaseSPtr msg,  WriteHeadersCallbackType cb)
{
}

void MessageWriter::end()
{
}
#pragma mark - internal functions
void MessageWriter::p_put_headers_stuff_in_buffer()
{
    MessageBaseSPtr msg = m_current_message;
    m_header_buf_sptr = serializeHeaders(*msg);
//    std::cout << m_header_buf.toString() << std::endl;
    LogDebug("request size: ");
}
//
// writes the entire body - precondition - we have the entire body already
//
void MessageWriter::p_async_write_full_body(WriteMessageCallbackType cb)
{
    LogDebug(" cb: ", (long) &cb);
    if( ( ! m_body_buffer_chain_sptr) || ( m_body_buffer_chain_sptr->size() == 0) ) {
        Marvin::ErrorType ee = Marvin::make_error_ok();
        auto pf = std::bind(cb, ee);
        m_io.post(pf);
    } else{
        m_write_sock->asyncWrite(m_body_buffer_chain_sptr, [this, cb](Marvin::ErrorType& ec, std::size_t bytes_transfered){
        auto pf = std::bind(cb, ec);
        m_io.post(pf);
        });
    }
}
} // namespace