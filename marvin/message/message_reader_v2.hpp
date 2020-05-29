#ifndef marvin_message_reader_v2_hpp
#define marvin_message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/message/full_msg_rdr.hpp>

namespace Marvin {

class MessageReaderV2;
typedef std::shared_ptr<MessageReaderV2> MessageReaderV2SPtr;
typedef std::unique_ptr<MessageReaderV2> MessageReaderV2UPtr;

/**
* \ingroup http
* \brief Knows how to read http messages in the form of MessageBase instances; note this class
*  is derived from MessageBase so the message read is bound to this reader as a subclass instance.
*
 */
class MessageReaderV2// : public Marvin::MessageBase
{
public:
    
    using SPtr = std::shared_ptr<MessageReaderV2>;

    MessageReaderV2(ISocketSPtr readSock);
    ~MessageReaderV2();

    using ReadMessageCallback = std::function<void(Marvin::ErrorType err)>;
    using ReadMessageHandler = std::function<void(Marvin::ErrorType err)>;
    void async_read_message(ReadMessageHandler cb);

    MessageBase::SPtr get_message_sptr();

protected:

    FullMessageReader           m_full_rdr;
    ISocketSPtr                 m_read_sock;
    MessageBase::SPtr           m_current_message_sptr;
};
} // namespcae
#endif
