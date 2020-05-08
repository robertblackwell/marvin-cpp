#ifndef marvin_message_reader_v2_hpp
#define marvin_message_reader_v2_hpp

#include <iostream>
#include <iterator>
#include <algorithm>

#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/connection/socket_interface.hpp>
#include <marvin/callback_typedefs.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/message/full_msg_rdr.hpp>

namespace Marvin {

class MessageReader;
typedef std::shared_ptr<MessageReader> MessageReaderSPtr;
typedef std::unique_ptr<MessageReader> MessageReaderUPtr;

/**
* \ingroup http
* \brief Knows how to read http messages in the form of MessageBase instances; note this class
*  is derived from MessageBase so the message read is bound to this reader as a subclass instance.
*
 */
class MessageReader : public Marvin::MessageBase
{
public:

    using ReadMessageCallback = std::function<void(Marvin::ErrorType err)>;
    using ReadBodyCallback = std::function<void(Marvin::ErrorType err, Marvin::BufferChainSPtr chunkSPtr)>;

    MessageReader(ISocketSPtr readSock);
    ~MessageReader();

    void readMessage(ReadMessageCallback cb);

    void readHeaders(std::function<void(Marvin::ErrorType err)> cb);
    void readBody(std::function<void(Marvin::ErrorType err, Marvin::BufferChainSPtr chunkSPtr)> bodyCb);

protected:

    FullMessageReader           m_full_rdr;
    ISocketSPtr                 m_read_sock;
};
} // namespcae
#endif
