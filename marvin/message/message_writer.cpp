#include <marvin/message/message_writer.hpp>

#include <marvin/buffer/buffer.hpp>
#include <marvin/error/marvin_error.hpp>
#include <exception>
#include <marvin/configure_trog.hpp>

namespace Marvin {

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

std::string trace_writer(MessageWriter& writer)
{
    std::stringstream ss;
//    ss  << traceMessageV2(writer)
//        << "[" << writer._haveContent << "]"
//        << " body.len: " << writer._bodyContent.size() ;
//    std::stringstream ss;
//    ss << "Just testing";
    return ss.str();
}

MessageWriter::MessageWriter(ISocketSPtr write_sock): m_io(write_sock->get_io_context()), m_write_sock(write_sock)
{
   TROG_TRACE_CTOR();
    m_current_message = nullptr;
    m_header_buf_sptr = nullptr;
}

MessageWriter::~MessageWriter()
{
   TROG_TRACE_CTOR();
}

#pragma mark - public methods
void MessageWriter::async_write(MessageBaseSPtr msg, std::string& body_string, WriteMessageHandler cb)
{
   
    if(body_string.size() == 0 ) {
        async_write(msg, cb);
    } else {
//        std:: cout << std::endl << "body buffer write: " << body_string << std::endl;
        m_body_buffer_string = body_string;
        m_body_buffer_chain_sptr = Marvin::makeBufferChainSPtr(body_string);
        async_write(msg, cb);
    }
}
void MessageWriter::async_write(MessageBaseSPtr msg, Marvin::ContigBuffer::SPtr body_mb_sptr, WriteMessageHandler cb)
{
    assert(body_mb_sptr != nullptr);
    m_body_buffer_chain_sptr = Marvin::makeBufferChainSPtr(body_mb_sptr);
    async_write(msg, cb);
}

void MessageWriter::async_write(MessageBaseSPtr msg, Marvin::BufferChain::SPtr body_chain_sptr, WriteMessageHandler cb)
{
    m_body_buffer_chain_sptr = body_chain_sptr;
    async_write(msg, cb);
}

void
MessageWriter::async_write(MessageBaseSPtr msg, WriteMessageHandler cb)
{
    TROG_DEBUG("");
    MessageBaseSPtr tmp = msg;
    m_current_message = msg;
    async_write_headers(msg, [this, cb](Marvin::ErrorType &ec)
    {
        TROG_DEBUG(" cb: ", (long) &cb);
        // doing a full write of the message
        if (ec) {
            TROG_DEBUG("", ec.value(), ec.category().name(), ec.category().message(ec.value()));
            cb(ec);
        } else {
            p_async_write_full_body([this, cb](Marvin::ErrorType &ec2)
                                    {
                                        TROG_DEBUG(" cb: ", (long) &cb);
                                        auto pf = std::bind(cb, ec2);
                                        cb(ec2);
                                    });
        }
    });
}

void
MessageWriter::async_write_headers(MessageBaseSPtr msg, WriteHeadersHandler cb)
{
    p_put_headers_stuff_in_buffer();

    m_write_sock->async_write(*m_header_buf_sptr, [this, cb](Marvin::ErrorType &ec, std::size_t bytes_transfered)
    {

        TROG_DEBUG("");
        // TODO need to check and do something about insufficient write
        auto pf = std::bind(cb, ec);
        cb(ec);
    });
}


void MessageWriter::async_write_body_data(std::string& data, WriteBodyDataHandler cb)
{
    auto bf = boost::asio::buffer(data);
    m_write_sock->async_write(bf, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}
void MessageWriter::async_write_body_data(Marvin::ContigBuffer& data, WriteBodyDataHandler cb)
{
    m_write_sock->async_write(data, [cb](Marvin::ErrorType &err, std::size_t bytes_transfered)
    {
        cb(err);
    });
}

void MessageWriter::async_write_body_data(boost::asio::const_buffer data, WriteBodyDataHandler cb)
{
    m_write_sock->async_write(data, [cb](Marvin::ErrorType& err, std::size_t bytes_transfered) {
        cb(err);
    });
}

void MessageWriter::async_write_trailers(MessageBaseSPtr msg, WriteHeadersHandler cb)
{
}

void MessageWriter::end()
{
}
#pragma mark - internal functions
void MessageWriter::p_put_headers_stuff_in_buffer()
{
    MessageBaseSPtr msg = m_current_message;
    m_header_buf_sptr = serialize_headers(*msg);
    std::string s = m_header_buf_sptr->toString();
//    std::cout << m_header_buf.toString() << std::endl;
    TROG_DEBUG("request size: ");
}
//
// writes the entire body - precondition - we have the entire body already
//
void MessageWriter::p_async_write_full_body(WriteMessageHandler cb)
{
    TROG_DEBUG(" cb: ", (long) &cb);
    if( ( ! m_body_buffer_chain_sptr) || ( m_body_buffer_chain_sptr->size() == 0) ) {
        Marvin::ErrorType ee = Marvin::make_error_ok();
        auto pf = std::bind(cb, ee);
        m_io.post(pf);
    } else{
        m_write_sock->async_write(m_body_buffer_chain_sptr,
                                  [this, cb](Marvin::ErrorType &ec, std::size_t bytes_transfered)
                                  {
                                      auto pf = std::bind(cb, ec);
                                      m_io.post(pf);
                                  });
    }
}
} // namespace