//
//  message_writer.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef message_writer_hpp
#define message_writer_hpp

#include <stdio.h>
#include "callback_typedefs.hpp"
#include "message.hpp"
#include "read_socket_interface.hpp"
#include "connection_interface.hpp"

class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterSPtr;
typedef std::unique_ptr<MessageWriter> MessageWriterUPtr;

class MessageWriter : public MessageBase{
public:

    MessageWriter(boost::asio::io_service& io, bool is_request);
    ~MessageWriter();
    
    void setContent(std::string& contentStr);
    
    void asyncWrite(WriteMessageCallbackType cb);
    void asyncWriteHeaders(WriteHeadersCallbackType cb);
    void asyncWriteBodyData(WriteBodyDataCallbackType cb);
    void asyncWriteTrailers(AsyncWriteCallbackType cb);
    void end();
    
    void setWriteSock(WriteSocketInterface* conn){ _writeSock = conn;}
    
    friend std::string traceWriter(MessageWriter& wrtr);

protected:
    void asyncWriteFullBody(WriteMessageCallbackType cb);

    void onWriteHeaders(Marvin::ErrorType& ec);
    void putHeadersStuffInBuffer();

    boost::asio::io_service&    _io;
    WriteSocketInterface*       _writeSock;
    
    bool                        _haveContent;
    boost::asio::streambuf      _bodyBuf;
    std::string                 _bodyContent;
    FBuffer*                    _currentBodyFBuffer;
    
    boost::asio::streambuf      _headerBuf;
};

#endif /* message_writer_hpp */
