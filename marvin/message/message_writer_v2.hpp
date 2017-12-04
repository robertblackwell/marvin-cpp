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
#include "read_socket_interface.hpp"
#include "connection_interface.hpp"
#include "tcp_connection.hpp"

class MessageWriterV2;
typedef std::shared_ptr<MessageWriterV2> MessageWriterV2SPtr;
typedef std::unique_ptr<MessageWriterV2> MessageWriterV2UPtr;

class MessageWriterV2
{
public:

    /**
    * The message writer has the logic to output MessageBase objects to an allready
    * open connection.
    * This is a one-shot object - once it has written a single message it should
    * be discarded and a new one used for the next message on the same connection
    */
//    MessageWriterV2(boost::asio::io_service& io, TCPConnection& conn);
    MessageWriterV2(boost::asio::io_service& io, ConnectionInterfaceSPtr conn);
    ~MessageWriterV2();
    
    void asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, std::string& body_string, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, MBufferSPtr body_mb_sptr, WriteMessageCallbackType cb);
    void asyncWrite(MessageBaseSPtr msg, BufferChainSPtr body_chain_sptr, WriteMessageCallbackType cb);

    void asyncWriteHeaders(MessageBaseSPtr msg, WriteHeadersCallbackType cb);

    void asyncWriteBodyData(std::string& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(MBuffer& data, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(BufferChainSPtr chain_ptr, WriteBodyDataCallbackType cb);
    void asyncWriteBodyData(boost::asio::const_buffer data, WriteBodyDataCallbackType cb);

    void asyncWriteTrailers(MessageBaseSPtr msg, AsyncWriteCallbackType cb);
    
    void end();
    
    friend std::string traceWriterV2(MessageWriterV2& wrtr);

protected:
    void asyncWriteFullBody(WriteMessageCallbackType cb);

    void onWriteHeaders(Marvin::ErrorType& ec);
    void putHeadersStuffInBuffer();

    boost::asio::io_service&    _io;
    ConnectionInterfaceSPtr     _conn;
    MessageBaseSPtr             _currentMessage;
    /**
    * @todo - raw pointers for MBuffer and FBuffer are a problem
    */
    MBuffer                     _m_header_buf;
    
    bool                        _haveContent;
    boost::asio::streambuf      _bodyBuf;
    
    MBufferSPtr                 _body_mbuffer_sptr;
    std::string                 _body_buffer_string;
//    BufferChain                 _body_buffer_chain;
    BufferChainSPtr             _body_buffer_chain_sptr;
    
};

#endif /* message_writer_hpp */
