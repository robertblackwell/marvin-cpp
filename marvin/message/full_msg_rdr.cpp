#include <marvin/message/full_msg_rdr.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

#include <marvin/connection/socket_interface.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelDebug|Trog::LogLevelTrace4|Trog::LogLevelTrace3)

namespace Marvin {

// FullMessageReader::MessageReader(ISocketSPtr read_sock, boost::asio::streambuf, FullMessageReader::DoneCallback cb)
FullMessageReader::FullMessageReader(ISocketSPtr read_socket_sptr)
: m_streambuffer()
{
   TROG_TRACE_CTOR();
    m_read_sock_sptr = read_socket_sptr;
}

FullMessageReader::~FullMessageReader()
{
   TROG_TRACE_CTOR();
}
#pragma mark -  simple public getters
/**
 * The possible outcomes:
 * -    got a message and err == ok, this is a good outcome process the message
 * -    got a message and err == eof, this is a good outcome process the message
 *                                      but no point trying to read another one
 * -    err == parse error, did not get a complete message so cannot process one
 *                          should reply to peer bad message format
 * -    err == err, just shutdown/close link
 * 
 */
void FullMessageReader::async_read_message(MessageBaseSPtr message_sptr, FullMessageReader::DoneCallback cb)
{
    throw("not implemented yet");
}
void FullMessageReader::async_read_message(MessageBase& message_ref, FullMessageReader::DoneCallback cb)
{
    MessageBase* p = std::addressof(message_ref);
    async_read_message(p, cb);
}
void FullMessageReader::async_read_message(MessageBase* message_ptr, FullMessageReader::DoneCallback cb)
{
    m_read_cb = cb;
    m_current_message_ptr =  message_ptr;
    m_parser.begin(m_current_message_ptr);
    if (m_streambuffer.data().size() > 0) {
        // there was already data in the streambuffer so parse it
        p_parse_some();
    } else {
        m_parser.begin(m_current_message_ptr);
        p_read_some();
    }
}
void FullMessageReader::p_read_some()
{
    /**
     *
     */
    std::size_t buf_size = recommended_buffer_size(*m_current_message_ptr, m_parser);
    auto mutablebuffer = m_streambuffer.prepare(buf_size);
    m_read_sock_sptr->async_read(mutablebuffer, [buf_size, this](Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        // TODO - do I need to call streambuffer.commmit()
        TROG_TRACE4("bytes read : ", bytes_transfered, " buf_size: ", buf_size)
        if (err) {
            TROG_DEBUG(err.message());
            // need to check for end of file - any error an zero bytes
            if(bytes_transfered == 0) {
                if (m_parser.started && (!m_parser.message_done)) {
                    // eof and started and message not done - treat as eof == eom
                    p_on_eof();
                } else {
                    // socket is just normal close
                    p_on_read_error(err);
                }
            } else {
                auto x = err.message();
                TROG_DEBUG("error : ", x);
                p_on_read_error(err);
            }
        } else {
            m_streambuffer.commit(bytes_transfered);
            p_parse_some();
        }
    });
}
void FullMessageReader::p_parse_some()
{
    while(m_streambuffer.data().size() > 0) {
        Parser::ReturnValue r = m_parser.consume(m_streambuffer);
        switch (r.return_code) {
            case Parser::ReturnCode::error:
                p_on_parse_error(m_parser.get_error());
            break;
            case Parser::ReturnCode::end_of_data:
            case Parser::ReturnCode::end_of_header:
                p_read_some();
            break;
            case Parser::ReturnCode::end_of_message:
                p_on_done();
            break;
        }
    }
}
void FullMessageReader::p_on_eof()
{
    ParserError pe;
    Parser::ReturnValue r = m_parser.end();
    switch (r.return_code) {
        case Parser::ReturnCode::error:
            pe = m_parser.get_error();
            p_on_parse_error(pe);
        break;
        case Parser::ReturnCode::end_of_data:
            pe = {.name="eof_error", .description="eof did not complete headers and message", .err_number = HPE_INVALID_EOF_STATE };
            p_on_parse_error(pe);
        break;
        case Parser::ReturnCode::end_of_header:
        case Parser::ReturnCode::end_of_message:
            p_on_done();
        break;
    }
}
void FullMessageReader::p_on_parse_error(ParserError pe)
{
    TROG_WARN("parser error: ", pe.name, pe.description);
    ErrorType err = Error::make_eparse();
    m_read_cb(err);
}
void FullMessageReader::p_on_read_error(ErrorType& err)
{
    TROG_WARN("read error: ", err.message());
    m_read_cb(err);    
}
void FullMessageReader::p_on_done()
{
    ErrorType err = Error::make_ok();
    m_read_cb(err);
}
} // namespace