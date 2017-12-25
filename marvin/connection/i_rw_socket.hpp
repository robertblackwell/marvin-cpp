//
//  i_socket.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef i_socket_h
#define i_socket_h
#include "buffer.hpp"
#include "marvin_error.hpp"
#include "callback_typedefs.hpp"

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallback;
typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncWriteCallback;

class IReadSocket;
typedef std::shared_ptr<IReadSocket> IReadSocketSPtr;

class IReadSocket{
public:
    virtual void asyncRead(MBuffer& mb, AsyncReadCallback cb) = 0;
};


class IWriteSocket;
typedef std::shared_ptr<IWriteSocket> IWriteSocketSPtr;

class IWriteSocket{
public:
//    virtual void asyncWrite(FBuffer& fb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(MBuffer& fb, AsyncWriteCallback) = 0;
    virtual void asyncWrite(BufferChainSPtr chain_sptr, AsyncWriteCallback) = 0;
    virtual void asyncWrite(boost::asio::const_buffer buf, AsyncWriteCallback cb) = 0;
    virtual void asyncWrite(boost::asio::streambuf& sb, AsyncWriteCallback) = 0;
};
#endif /* i_socket_h */
