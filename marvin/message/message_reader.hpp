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
    
    using SPtr = std::shared_ptr<MessageReader>;

    MessageReader(ISocketSPtr readSock);
    ~MessageReader();

    using ReadMessageCallback = std::function<void(Marvin::ErrorType err)>;
    using ReadMessageHandler = std::function<void(Marvin::ErrorType err)>;
    void async_read_message(ReadMessageHandler cb);

// have not deviced how to implement streaning read yet
#ifdef  MARVIN_MSGREADER_PARTIAL_READ
    using ReadBodyCallback = std::function<void(Marvin::ErrorType err, Marvin::BufferChain::SPtr chunkSPtr)>;
    using ReadHeadersHandler = std::function<void(Marvin::ErrorType err)>;
    using ReadBodyHandler  = std::function<void(Marvin::ErrorType err, Marvin::BufferChain::SPtr chunkSPtr)>;
    void asyn_read_headers(std::function<void(Marvin::ErrorType err)> cb);
    void async_read_body(std::function<void(Marvin::ErrorType err, Marvin::BufferChain::SPtr chunkSPtr)> bodyCb);
#endif

protected:

    FullMessageReader           m_full_rdr;
    ISocketSPtr                 m_read_sock;
};
} // namespcae
#endif
