#ifndef marvin_full_message_reader_v2_hpp
#define marvin_full_message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

namespace Marvin {

class FullMessageReader
{
public:

    using SPtr = std::shared_ptr<FullMessageReader>;
    using UPtr = std::unique_ptr<FullMessageReader>;
    using DoneCallback = std::function<void(Marvin::ErrorType err)>;

    FullMessageReader(ISocketSPtr read_socket_sptr);
    ~FullMessageReader();

    void readMessage(MessageBase& message, DoneCallback cb);
    void readMessage(MessageBase* message, DoneCallback cb);
    void readMessage(MessageBaseSPtr message, DoneCallback cb);

protected:
    //----------------------------------------------------------------------------------------------------
    // protected methods
    //----------------------------------------------------------------------------------------------------
    void p_read_some();
    void p_parse_some();
    void p_on_read_error(ErrorType& err);
    void p_on_parse_error(ParserError pe);
    void p_on_eof();
    void p_on_done();
    std::size_t p_buffer_strategy(MessageBase& partial_msg, Parser& p);

    ISocketSPtr                  m_read_sock_sptr;
    Parser                       m_parser;
    boost::asio::streambuf       m_streambuffer;
    MessageBase*                 m_current_message_ptr;
    DoneCallback                 m_read_cb;
    std::size_t                  m_current_body_buffer_size;
};
} // namespcae
#endif
