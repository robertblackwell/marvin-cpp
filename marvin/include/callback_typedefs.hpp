//
//  callback_typedefs.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/10/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef callback_typedefs_h
#define callback_typedefs_h
#include "marvin_error.hpp"
#include "bufferV2.hpp"


class ConnectionInterface;
typedef std::function<void(Marvin::ErrorType& err)>                    ErrorOnlyCallbackType;

typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncReadCallbackType;
typedef std::function<void(Marvin::ErrorType& er, std::size_t bytes_transfered)> AsyncWriteCallbackType;

typedef std::function<void(Marvin::ErrorType& err, ConnectionInterface* conn)>  ConnectCallbackType;

typedef std::function<void(Marvin::ErrorType& err)>                    WriteHeadersCallbackType;

/// Callback when writing body data
//typedef std::function<void(Marvin::ErrorType& err, FBuffer* fBufPtr)>  WriteBodyDataCallbackType;
typedef std::function<void(Marvin::ErrorType& err)>                     WriteBodyDataCallbackType;

/// Call back when reading full message is complete
typedef std::function<void(Marvin::ErrorType& err)>                    WriteMessageCallbackType;


//
// callback types required by MessageReader
//

/// call back when reading headers
typedef std::function<void(Marvin::ErrorType& err)>                    ReadHeadersCallbackType;

/// Callback when reading body data
typedef std::function<void(Marvin::ErrorType& err, FBuffer* fBufPtr)>  ReadBodyDataCallbackType;

/// Call back when reading full message is complete
typedef std::function<void(Marvin::ErrorType& err)>                    ReadMessageCallbackType;


#endif /* callback_typedefs_h */
