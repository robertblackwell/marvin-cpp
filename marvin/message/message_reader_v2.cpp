#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>


#include <marvin/message/message_reader_v2.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/error_handler/error_handler.hpp>


#include <marvin/connection/socket_interface.hpp>

namespace Marvin {


MessageReaderV2::MessageReaderV2(ISocketSPtr read_sock)
: m_read_sock(read_sock), m_full_rdr(read_sock)
{
   TROG_TRACE_CTOR();
}


/**
* Destructor - nothing to do. All pointers held by an instance
* are smart pointers.
*/
MessageReaderV2::~MessageReaderV2()
{
   TROG_TRACE_CTOR();
}

MessageBase::SPtr MessageReaderV2::get_message_sptr()
{
    if (m_current_message_sptr == nullptr) {
        MARVIN_THROW("probably should not be getting current message when the sptr is null")
    }
    return m_current_message_sptr;
}

void MessageReaderV2::async_read_message(std::function<void(Marvin::ErrorType)> cb)
{
    m_current_message_sptr = std::make_shared<MessageBase>();
    m_full_rdr.async_read_message(m_current_message_sptr, [this, cb](ErrorType err)
    {
        /**
         * \TODO - make this a post
         */
        cb(err);
    });
}

} // namespace