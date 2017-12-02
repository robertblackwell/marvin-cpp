
#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include "boost_stuff.hpp"
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "bufferV2.hpp"
#include "message.hpp"
#include "parser.hpp"
#include "rb_logger.hpp"

RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

//#include "repeating_timer.hpp"
//#include "mock_rsockection.hpp"
#include "read_socket_interface.hpp"
#include "message_reader_v2.hpp"

#pragma mark - tracing functions
std::string traceReader(MessageReaderV2& rdr)
{
    std::stringstream ss;
    ss  << traceMessage(rdr)
        << " body.len: " << rdr.body.size() ;
    return ss.str();
}
#pragma mark - class default constants
std::size_t MessageReaderV2::__headerBufferSize = 10000;
std::size_t MessageReaderV2::__bodyBufferSize = 20000;

#pragma mark - static functions to do once only config for the class
/**
* Called at program startup to override the default value
* for the body buffer
*/
void MessageReaderV2::configSet_BodyBufferSize(long bsize)
{
    __bodyBufferSize = bsize;
}

/**
* Called at program startup to override the default value
* for the haeder buffer
*/
void MessageReaderV2::configSet_HeaderBufferSize(long bsize)
{
    __headerBufferSize = bsize;
}
#pragma mark - constructor

MessageReaderV2::MessageReaderV2(ReadSocketInterface* readSock, boost::asio::io_service& io): _io(io), _readSock(readSock)
{
    LogTorTrace();
    _body_buffer_size   = __bodyBufferSize;
    _header_buffer_size = __headerBufferSize;
    _header_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_header_buffer_size));
}

/**
* Destructor - nothing to do. All pointers held by an instance
* are smart pointers.
*/
MessageReaderV2::~MessageReaderV2()
{
    LogTorTrace();
    // how to know what to get rid of
    // delete _readBuffer;
}
#pragma mark -  simple public getters

/*!
* accesses the BufferChain containing the de-chunked body data
*/
BufferChain MessageReaderV2::get_body_chain()
{
    return _body_buffer_chain;
}
/*!
* accesses the BufferChain containing the raw (not de-chunked) body data
*/
BufferChain MessageReaderV2::get_raw_body_chain()
{
    return _raw_body_buffer_chain;
}
#pragma mark - public interface read methods
/**
* An interface method that is called to initiate the read of an entire
* message
*/
void MessageReaderV2::readMessage(std::function<void(Marvin::ErrorType)> cb)
{
    this->read_message(cb);
}

/*!
* An interface method called to initiate the reading of message first line and headers
* Completion signalled through the callback
*/
void MessageReaderV2::readHeaders(std::function<void(Marvin::ErrorType)> cb)
{
    read_headers(cb);
}
/*!
* An interface method that is called to initiate an async read of some body data.
* Result is returned via the callback
*/
void MessageReaderV2::readBody(std::function<void(Marvin::ErrorType err, BufferChain chunk)> cb)
{
    _read_body_cb = cb;
    _reading_body = true;
    if (_body_buffer_chain.size() > 0) {
        post_body_chunk_cb(Marvin::make_error_ok(), _body_buffer_chain);
        _body_buffer_chain.clear();
    } else {
        _read_body_chunk();
    }
}

#pragma mark - Parser virtual overrides - catch parser events

//void OnParserBegin(){ LogDebug("");}
void MessageReaderV2::OnMessageComplete(MessageInterface* msg){
    LogDebug("");
//    std::cout << "message complete" << std::endl;
}
/**
* @note WARNING - http_parser has been modified. The interface to this function
* and that of headers_complete_cb required by http_parser have been modified (and
* that required a few lines of change in http_parser) to provide the address in the
* current buffer of the first byte of body data in that same buffer, together
* with the length of the unparsed buffer at the time of headers complete.
* This is the only way of capturing a chunk header that happened to be in the
* same buffer as the last of the header data.
* If remander is  NOT zero then some body data (maybe only chunk header)
* was in the buffer with the last of the header data.
*/
void MessageReaderV2::OnHeadersComplete(MessageInterface* msg, void* body_start_ptr, std::size_t remainder)
{
    if (remainder > 0) {
        MBufferSPtr tmp = std::shared_ptr<MBuffer>(new MBuffer(remainder));
        tmp->append(body_start_ptr, remainder);
        _raw_body_buffer_chain.push_back(tmp);
    }
    LogDebug("");
}

MessageInterface* MessageReaderV2::currentMessage(){  return this; }

/**
* Overrides a Parser virtual method.
* Called whenever the http_parser sees a piece of body data
*
* This function is the only place where each piece of body data whether chunked or not
* is seen. Hence the only place to catch body data reliably.
*/
void MessageReaderV2::OnBodyData(void* buf, int len)
{
    MBufferSPtr tmp = std::shared_ptr<MBuffer>(new MBuffer(len));
    tmp->append(buf, len);
    _body_buffer_chain.push_back(tmp);
}

void MessageReaderV2::OnChunkBegin(int chunkLength) { LogDebug("");}
void MessageReaderV2::OnChunkData(void* buf, int len){ LogDebug("");}
void MessageReaderV2::OnChunkEnd() { LogDebug(""); }

#pragma mark - methods for reading headers as part of reading an entire message or just reading the headers

/**
* Called interface method readMessage to start the reading of headers and
* followed by the reading of all body data.
* Note the setting of _reading_full_message = true
*/
void MessageReaderV2::read_message(std::function<void(Marvin::ErrorType err)> cb)
{
    _reading_full_message = true;
    _read_message_cb = cb;
    this->_read_some_headers();
}
/**
* called by interface method readHeaders to start the reading of headers without
* reading body data.
* Note the setting of _reading_full_message = false
*/
void MessageReaderV2::read_headers(std::function<void(Marvin::ErrorType err)> cb)
{
    _reading_full_message = false;
    _read_message_cb = cb;
    this->_read_some_headers();
}
/**
* The first step in a two part async loop that reads all headers (until headersComplete)
* This function sets up and initiates a read.
*/
void MessageReaderV2::_read_some_headers()
{
    LogDebug(" fd: ", _readSock->nativeSocketFD());
    assert( _header_buffer_sptr != nullptr );
    auto h = std::bind(&MessageReaderV2::_handle_header_read, this, std::placeholders::_1, std::placeholders::_2);
    _readSock->asyncRead(*_header_buffer_sptr, h);
}
/**
* Step two in an async loop to read all headers. This is the async completion
* handler for reading headers
*/
void MessageReaderV2::_handle_header_read(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", _readSock->nativeSocketFD());
    LogDebug("er: ", er.message());
    /**
    * an io error with bytes_transfered == 0 is probably EOF - let the parser handle it
    * otherwise (err && (bytes_transfered > 0)) return with error
    */
    if(er && (bytes_transfered > 0)) {
        post_message_cb(er);
        LogError("", er.message());
    }

    _header_buffer_sptr->setSize(bytes_transfered);
    MBuffer& mb = *_header_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! parser_ok(nparsed, mb)) {
        post_message_cb(Marvin::make_error_parse());
        return;
    }
    

    if( isFinishedMessage() ) {
        post_message_cb(Marvin::make_error_ok());
    } else if( isFinishedHeaders()& (! isFinishedMessage())) {
        if( _reading_full_message ) {
            _read_all_body();
        } else {
            post_message_cb(Marvin::make_error_ok());
        }
    } else if( ! isFinishedHeaders() ) {
        _read_some_headers();
    }
}
#pragma mark - methods for reading all body data as part of reading an entire message
/**
* Kicks off the process of reading all body data.
*/
void MessageReaderV2::_read_all_body()
{
//    _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    _make_new_body_buffer();
    _read_some_body();
}
/**
* First part of an async loop that reads all body data. This method
* sets up and initiates an async read
*/
void MessageReaderV2::_read_some_body()
{
    LogDebug(" fd: ", _readSock->nativeSocketFD());
    auto h = std::bind(&MessageReaderV2::_handle_body_read, this, std::placeholders::_1, std::placeholders::_2);
    _readSock->asyncRead(*_body_buffer_sptr, h);
}
/**
* Step two of the async loop that reads all body data. This is the async
* completion handler.
*/
void MessageReaderV2::_handle_body_read(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", _readSock->nativeSocketFD());
    /**
    * an io error with bytes_transfered == 0 is probably EOF - let the parser handle it
    * otherwise (err && (bytes_transfered > 0)) return with error
    */
    if(er && (bytes_transfered > 0)) {
        post_message_cb(er);
        LogError("", er.message());
    }
    _body_buffer_sptr->setSize(bytes_transfered);
    MBufferSPtr tmp = std::shared_ptr<MBuffer>(new MBuffer(bytes_transfered));
    _raw_body_buffer_chain.push_back(tmp);
    
    MBuffer& mb = *_body_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! parser_ok(nparsed, mb)) {
        post_message_cb(Marvin::make_error_parse());
        return;
    }
    
    if( isFinishedMessage()) {
        post_message_cb(Marvin::make_error_ok());
    } else {
        _make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
        _read_some_body();
    }
}

#pragma mark - methods for reading a chunk of body data and implementing readData
/**
* Called by interface method readBody()
* Initiates the process of reading ONE chunk of body data. Sets up
* a read and completion handler. No looping for this
*/
void MessageReaderV2::_read_body_chunk()
{
    LogDebug(" fd: ", _readSock->nativeSocketFD());
    auto h = std::bind(&MessageReaderV2::_handle_body_chunk, this, std::placeholders::_1, std::placeholders::_2);
    if(_body_buffer_sptr == nullptr) {
        _make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    }
    _readSock->asyncRead(*_body_buffer_sptr, h);
}
/**
* Completion handler for reading a single chunk of body data.
*/
void MessageReaderV2::_handle_body_chunk(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", _readSock->nativeSocketFD());
    /**
    * an io error with bytes_transfered == 0 is probably EOF - let the parser handle it
    * otherwise (err && (bytes_transfered > 0)) return with error
    */
    if(er && (bytes_transfered > 0)) {
        post_body_chunk_cb(er, _body_buffer_chain);
        LogError("", er.message());
    }
    
    _body_buffer_sptr->setSize(bytes_transfered);
    MBufferSPtr tmp = std::shared_ptr<MBuffer>(new MBuffer(bytes_transfered));
    _raw_body_buffer_chain.push_back(tmp);
    
    MBuffer& mb = *_body_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! parser_ok(nparsed, mb)) {
        post_body_chunk_cb(Marvin::make_error_parse(), _body_buffer_chain);
        return;
    }
    

    if( isFinishedMessage()) {
        post_body_chunk_cb(Marvin::make_error_eom(), _body_buffer_chain);
        _body_buffer_chain.clear();
    } else {
        post_body_chunk_cb(Marvin::make_error_ok(), _body_buffer_chain);
        _body_buffer_chain.clear();
//        _make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    }
}
#pragma mark - buffer management
/**
* Creates a shared pointer to a single new buffer for reading body data.
*/
void MessageReaderV2::_make_new_body_buffer()
{
    _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
}

#pragma mark - post method for scheduling a callback to run later on the runloop
/**
* Schedules the execution of the _read_message_cb in the future on this instances
* io_service or runloop.
* _read_message_cb is a property that stores the address of the
* callback provided to readMessage() and readHeaders() interface methods.
*/
void MessageReaderV2::post_message_cb(Marvin::ErrorType er)
{
    auto pf = std::bind(_read_message_cb, er);
    _io.post(pf);
}
/**
* Schedules the execution of the _read_message_cb in the future on this instances
* io_service or runloop.
* _read_body_cb is a property that stores the address of the
* callback provided to readBody();
*/
void MessageReaderV2::post_body_chunk_cb(Marvin::ErrorType er, BufferChain chain)
{
    auto pf = std::bind(_read_body_cb, er, chain);
    _io.post(pf);
}
#pragma mark - error related functions
bool MessageReaderV2::parser_ok(int nparsed, MBuffer& mb)
{
    if( nparsed != (int)mb.size()) {
        LogWarn("some next message in buffer");
    }
    /**
    * if parser status is OK or if (http_parser->errno == HPE_PAUSED && isFinishedMessage())
    * then we are OK with this message - parsing is paused on EOM
    * otherwise parser errors should be returned as errors
    */
    ParserError perr = this->getError();
    if( (perr.err_number == HPE_OK) || (perr.err_number == HPE_PAUSED && isFinishedMessage())) {
        // do nothing
        return true;
    } else {
        LogError("parse error", perr.err_number, perr.description) ;
//        post_message_cb(Marvin::make_error_parse());
        return false;
    }
}
