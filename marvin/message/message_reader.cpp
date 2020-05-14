#include <marvin/message/message_reader.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/macros.hpp>
#include <marvin/configure_trog.hpp>

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

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
void MessageReader::readMessage(std::function<void(Marvin::ErrorType)> cb)
{
    m_full_rdr.readMessage(*this, [this, cb](ErrorType err)
    {
        /**
         * \TODO - make this a post
         */
        cb(err);
    });
}
} // namespace