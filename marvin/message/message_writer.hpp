
#ifndef marvin_message_writer_v2_hpp
#define marvin_message_writer_v2_hpp

#include <cstdio>
#include <marvin/callback_typedefs.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/connection/socket_interface.hpp>

namespace Marvin {

class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterSPtr;
typedef std::unique_ptr<MessageWriter> MessageWriterUPtr;

/**
* \ingroup http
* \brief MessageWriter knows how to write a http message in the form of a MessageBase instance.
*
* Two modes of operation are provided:
*
*   -   take an entire message including (if there is any) the message body and
*       notify the caller when all is sent
*   -   write the entire message header in a single method call and then write
*       the message content or body in chunks
*
* Instance of this class are re-useable in that a single instance can write multiple messages one after the other.
*
* \warning this last mode is not fully implemented
*/
class MessageWriter
{
public:

    MessageWriter(ISocketSPtr conn);
    ~MessageWriter();
    
    void asyncWrite(Marvin::MessageBaseSPtr msg, WriteMessageCallbackType cb);
    void asyncWrite(Marvin::MessageBaseSPtr msg, std::string& body_string, WriteMessageCallbackType cb);
    void asyncWrite(Marvin::MessageBaseSPtr msg, Marvin::ContigBufferSPtr body_mb_sptr, WriteMessageCallbackType cb);
    void asyncWrite(Marvin::MessageBaseSPtr msg, Marvin::BufferChainSPtr body_chain_sptr, WriteMessageCallbackType cb);

    void asyncWriteHeaders(Marvin::MessageBaseSPtr msg, WriteHeadersCallbackType cb);
    void asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::ContigBuffer& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::BufferChainSPtr chain_ptr, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(boost::asio::const_buffer data, WriteBodyDataCallbackType cb);

    void asyncWriteTrailers(Marvin::MessageBaseSPtr msg, WriteHeadersCallbackType cb);
    
    void end();
    
    friend std::string traceWriter(MessageWriter& wrtr);

protected:
    void p_async_write_full_body(WriteMessageCallbackType cb);
    void p_put_headers_stuff_in_buffer();

    boost::asio::io_service&    m_io;
    ISocketSPtr                     m_write_sock;
    Marvin::MessageBaseSPtr   m_current_message;
    Marvin::ContigBufferSPtr             m_header_buf_sptr;
    Marvin::ContigBufferSPtr             m_body_mbuffer_sptr;
    std::string                     m_body_buffer_string;
    Marvin::BufferChainSPtr         m_body_buffer_chain_sptr;
    
};
} // namespcae
#endif /* message_writer_hpp */
