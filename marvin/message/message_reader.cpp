#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>


#include <marvin/message/message_reader.hpp>

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


MessageReader::MessageReader(ISocketSPtr read_sock)
: m_read_sock(read_sock), m_full_rdr(read_sock)
{
   TROG_TRACE_CTOR();
}

/**
* Destructor - nothing to do. All pointers held by an instance
* are smart pointers.
*/
MessageReader::~MessageReader()
{
   TROG_TRACE_CTOR();
}
void MessageReader::async_read_message(std::function<void(Marvin::ErrorType)> cb)
{
    m_full_rdr.async_read_message(*this, [this, cb](ErrorType err)
    {
        /**
         * \TODO - make this a post
         */
        cb(err);
    });
}
} // namespace