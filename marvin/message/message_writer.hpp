//
//  message_writer.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef message_writer_v2_hpp
#define message_writer_v2_hpp

#include <stdio.h>
#include "callback_typedefs.hpp"
#include "message.hpp"
#include "i_socket.hpp"
#include "i_socket.hpp"
#include "tcp_connection.hpp"

class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterSPtr;
typedef std::unique_ptr<MessageWriter> MessageWriterUPtr;

/**
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

    MessageWriter(boost::asio::io_service& io, ISocketSPtr conn);
    ~MessageWriter();
    
    void asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, std::string& body_string, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, Marvin::MBufferSPtr body_mb_sptr, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, Marvin::BufferChainSPtr body_chain_sptr, WriteMessageCallbackType cb);

    void asyncWriteHeaders(MessageBaseSPtr msg, WriteHeadersCallbackType cb);
    void asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::MBuffer& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(Marvin::BufferChainSPtr chain_ptr, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(boost::asio::const_buffer data, WriteBodyDataCallbackType cb);

    void asyncWriteTrailers(MessageBaseSPtr msg, AsyncWriteCallbackType cb);
    
    void end();
    
    friend std::string traceWriter(MessageWriter& wrtr);

protected:
    void p_async_write_full_body(WriteMessageCallbackType cb);
    void p_put_headers_stuff_in_buffer();

    boost::asio::io_service&    m_io;
    ISocketSPtr                 m_write_sock;
    MessageBaseSPtr             m_current_message;
    Marvin::MBuffer             m_header_buf;
    Marvin::MBufferSPtr         m_body_mbuffer_sptr;
    std::string                 m_body_buffer_string;
    Marvin::BufferChainSPtr     m_body_buffer_chain_sptr;
    
};

#endif /* message_writer_hpp */
