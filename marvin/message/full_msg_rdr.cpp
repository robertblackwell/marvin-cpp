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
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

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
void FullMessageReader::readMessage(MessageBaseSPtr message_sptr, FullMessageReader::DoneCallback cb)
{
    throw("not implemented yet");
}
void FullMessageReader::readMessage(MessageBase& message_ref, FullMessageReader::DoneCallback cb)
{
    MessageBase* p = std::addressof(message_ref);
    readMessage(p, cb);
}
void FullMessageReader::readMessage(MessageBase* message_ptr, FullMessageReader::DoneCallback cb)
{
    m_read_cb = cb;
    m_current_message_ptr =  message_ptr;
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
    m_read_sock_sptr->asyncRead(m_streambuffer, [this](Marvin::ErrorType& err, std::size_t bytes_transfered)
    {
        // TODO - do I need to call streambuffer.commmit()
        if (err) {
            // need to check for end of file
            if((bytes_transfered == 0) && (err == boost::asio::error::eof)) {
                if (m_parser.started && (!m_parser.message_done)) {
                    // eof and started and message not done - treat as eof == eom
                    p_on_eof();
                } else {
                    // socket is just normal close
                    p_on_read_error(err);
                }
            } else {
                p_on_read_error(err);
            }
        } else {
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
                p_on_parse_error(m_parser.getError());
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
            pe =  m_parser.getError();
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