
#ifndef marvin_message_writer_v2_hpp
#define marvin_message_writer_v2_hpp

#include <cstdio>
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

    using SPtr = std::shared_ptr<MessageWriter>;

    using ErrRefHandler = std::function<void(ErrorType& err)>;
    using WriteMessageHandler = ErrRefHandler ;
    using WriteHeadersHandler = ErrRefHandler ;
    using WriteBodyDataHandler = ErrRefHandler ;

    MessageWriter(ISocketSPtr conn);
    ~MessageWriter();
    
    void async_write(Marvin::MessageBaseSPtr msg, WriteMessageHandler cb);
    void async_write(Marvin::MessageBaseSPtr msg, std::string& body_string, WriteMessageHandler cb);
    void async_write(Marvin::MessageBaseSPtr msg, Marvin::ContigBuffer::SPtr body_mb_sptr, WriteMessageHandler cb);
    void async_write(Marvin::MessageBaseSPtr msg, Marvin::BufferChain::SPtr body_chain_sptr, WriteMessageHandler cb);

    void async_write_headers(Marvin::MessageBaseSPtr msg, WriteHeadersHandler cb);
    void async_write_body_data(std::string& data, WriteBodyDataHandler cb);
    void async_write_body_data(Marvin::ContigBuffer& data, WriteBodyDataHandler cb);
    void async_write_body_data(Marvin::BufferChain::SPtr chain_ptr, WriteBodyDataHandler cb);
    void async_write_body_data(boost::asio::const_buffer data, WriteBodyDataHandler cb);
    void async_write_trailers(Marvin::MessageBaseSPtr msg, WriteHeadersHandler cb);
    void end();

protected:
    void p_async_write_full_body(WriteMessageHandler cb);
    void p_put_headers_stuff_in_buffer();

    boost::asio::io_service&    m_io;
    ISocketSPtr                     m_write_sock;
    Marvin::MessageBaseSPtr   m_current_message;
    Marvin::ContigBuffer::SPtr             m_header_buf_sptr;
    Marvin::ContigBuffer::SPtr             m_body_mbuffer_sptr;
    std::string                     m_body_buffer_string;
    Marvin::BufferChain::SPtr         m_body_buffer_chain_sptr;
    
};
} // namespcae
#endif /* message_writer_hpp */
