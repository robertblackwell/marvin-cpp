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

class MessageWriterV2{
public:

    /**
    * The message writer has the logic to output MessageBase objects to an allready
    * open connection.
    */
    MessageWriterV2(boost::asio::io_service& io, TCPConnection& conn);
    ~MessageWriterV2();
    
    void asyncWrite(MessageBaseSPtr msg, WriteMessageCallbackType cb);
    
    void asyncWriteHeaders(MessageBaseSPtr msg, WriteHeadersCallbackType cb);
    void asyncWriteBodyData(void* data, WriteBodyDataCallbackType cb);
    void asyncWriteTrailers(MessageBaseSPtr msg, AsyncWriteCallbackType cb);
    
    void end();
    
    friend std::string traceWriterV2(MessageWriterV2& wrtr);

protected:
    void asyncWriteFullBody(WriteMessageCallbackType cb);

    void onWriteHeaders(Marvin::ErrorType& ec);
    void putHeadersStuffInBuffer();

    boost::asio::io_service&    _io;
    TCPConnection&              _conn;
    
    MessageBaseSPtr             _currentMessage;
    bool                        _haveContent;
    /**
    * @todo - raw pointers for MBuffer and FBuffer are a problem
    */
    MBuffer*                   _m_header_buf;
    boost::asio::streambuf      _bodyBuf;
    std::string                 _bodyContent;
//    FBuffer*                    _currentBodyFBuffer;
    
};

#endif /* message_writer_hpp */
