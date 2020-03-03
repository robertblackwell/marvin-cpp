
#include <iostream>
#include <iterator>
#include <algorithm>
#include<marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include<marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include <marvin/connection/socket_interface.hpp>
#include <marvin/message/message_reader.hpp>

 using namespace Marvin;
 using namespace Marvin::Http;



#pragma mark - tracing functions
std::string traceReader(MessageReader& rdr)
{
    std::stringstream ss;
    ss  << traceMessage(rdr)
        << " body.len: " << rdr.m_body.size() ;
    return ss.str();
}
#pragma mark - class default constants
std::size_t MessageReader::s_headerBufferSize = 10000;
std::size_t MessageReader::s_bodyBufferSize = 20000;

#pragma mark - static functions to do once only config for the class
/**
* Called at program startup to override the default value
* for the body buffer
*/
void MessageReader::configSet_BodyBufferSize(long bsize)
{
    s_bodyBufferSize = bsize;
}

/**
* Called at program startup to override the default value
* for the haeder buffer
*/
void MessageReader::configSet_HeaderBufferSize(long bsize)
{
    s_headerBufferSize = bsize;
}
#pragma mark - constructor

MessageReader::MessageReader( boost::asio::io_service& io, ISocketSPtr read_sock)
: m_io(io), m_read_sock(read_sock)
{
    LogTorTrace();
    m_body_buffer_size   = s_bodyBufferSize;
    m_header_buffer_size = s_headerBufferSize;
    m_header_buffer_sptr = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(m_header_buffer_size));
    m_body_buffer_chain_sptr = std::make_shared<Marvin::BufferChain>();
//    this->setBody(m_body_buffer_chain_sptr); /// \warning - this also sets content-length which is wrong
    m_body_chain_sptr = m_body_buffer_chain_sptr; // synchronizes reader buffer and message content
    m_raw_body_buffer_chain_sptr = std::make_shared<Marvin::BufferChain>();
}

/**
* Destructor - nothing to do. All pointers held by an instance
* are smart pointers.
*/
MessageReader::~MessageReader()
{
    LogTorTrace();
    // how to know what to get rid of
    // delete _readBuffer;
}
#pragma mark -  simple public getters

/*!
* accesses the BufferChain containing the de-chunked body data
*
Marvin::BufferChain MessageReader::get_body_chain()
{
    return m_body_buffer_chain;
}*/
/*!
* accesses the BufferChain containing the raw (not de-chunked) body data
*
Marvin::BufferChain MessageReader::get_raw_body_chain()
{
    return m_raw_body_buffer_chain;
}*/
#pragma mark - public interface read methods
/**
* An interface method that is called to initiate the read of an entire
* message
*/
void MessageReader::readMessage(std::function<void(Marvin::ErrorType)> cb)
{
    this->p_read_message(cb);
}

/*!
* An interface method called to initiate the reading of message first line and headers
* Completion signalled through the callback
*/
void MessageReader::readHeaders(std::function<void(Marvin::ErrorType)> cb)
{
    p_read_headers(cb);
}
/*!
* An interface method that is called to initiate an async read of some body data.
* Result is returned via the callback
*/
void MessageReader::readBody(ReadBodyCallback cb)
//void MessageReader::readBody(std::function<void(Marvin::ErrorType err, Marvin::BufferChainSPtr chunkSPtr)> cb)
{
//    assert(false); /// not yet tested
    m_read_body_cb = cb;
    m_reading_body = true;
    if (m_body_buffer_chain_sptr->size() > 0) {
       p_post_body_chunk_cb(Marvin::make_error_ok(), m_body_buffer_chain_sptr);
       m_body_buffer_chain_sptr = std::make_shared<Marvin::BufferChain>();
//        m_body_buffer_chain_sptr->clear();
    } else {
        p_read_body_chunk();
    }
}

#pragma mark - Parser virtual overrides - catch parser events

//void OnParserBegin(){ LogDebug("");}
void MessageReader::OnMessageComplete(MessageInterface* msg){
    LogDebug("");
//    std::cout << "message complete" << std::endl;
}
/**
* @WARNING - http_parser has been modified. The interface to this function
* and that of headers_complete_cb required by http_parser have been modified (and
* that required a few lines of change in http_parser) to provide the address in the
* current buffer of the first byte of body data in that same buffer, together
* with the length of the unparsed buffer at the time of headers complete.
* This is the only way of capturing a chunk header that happened to be in the
* same buffer as the last of the header data.
* If remander is  NOT zero then some body data (maybe only chunk header)
* was in the buffer with the last of the header data.
*/
void MessageReader::OnHeadersComplete(MessageInterface* msg, void* body_start_ptr, std::size_t remainder)
{
    if (remainder > 0) {
        Marvin::MBufferSPtr tmp = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(remainder));
        tmp->append(body_start_ptr, remainder);
        m_raw_body_buffer_chain_sptr->push_back(tmp);
    }
    LogDebug("");
}

MessageInterface* MessageReader::currentMessage(){  return this; }

/**
* Overrides a Parser virtual method.
* Called whenever the http_parser sees a piece of body data
*
* This function is the only place where each piece of body data whether chunked or not
* is seen. Hence the only place to catch body data reliably.
*/
void MessageReader::OnBodyData(void* buf, int len)
{
    Marvin::MBufferSPtr tmp = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(len));
    tmp->append(buf, len);
    m_body_buffer_chain_sptr->push_back(tmp);
}

void MessageReader::OnChunkBegin(int chunkLength) { LogDebug("");}
void MessageReader::OnChunkData(void* buf, int len){ LogDebug("");}
void MessageReader::OnChunkEnd() { LogDebug(""); }

#pragma mark - methods for reading headers as part of reading an entire message or just reading the headers

/**
* Called interface method readMessage to start the reading of headers and
* followed by the reading of all body data.
* Note the setting of _reading_full_message = true
*/
void MessageReader::p_read_message(std::function<void(Marvin::ErrorType err)> cb)
{
    m_reading_full_message = true;
    m_read_message_cb = cb;
    m_total_bytes_read = 0;
    this->p_read_some_headers();
}
/**
* called by interface method readHeaders to start the reading of headers without
* reading body data.
* Note the setting of _reading_full_message = false
*/
void MessageReader::p_read_headers(std::function<void(Marvin::ErrorType err)> cb)
{
    m_reading_full_message = false;
    m_read_message_cb = cb;
    m_total_bytes_read = 0;
    this->p_read_some_headers();
}
/**
* The first step in a two part async loop that reads all headers (until headersComplete)
* This function sets up and initiates a read.
*/
void MessageReader::p_read_some_headers()
{
    LogDebug(" fd: ", m_read_sock->nativeSocketFD());
    assert( m_header_buffer_sptr != nullptr );
    auto h = std::bind(&MessageReader::p_handle_header_read, this, std::placeholders::_1, std::placeholders::_2);
    m_read_sock->asyncRead(m_header_buffer_sptr, h);
}
/**
* Step two in an async loop to read all headers. This is the async completion
* handler for reading headers
*/
void MessageReader::p_handle_header_read(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", m_read_sock->nativeSocketFD());
    LogDebug("er: ", er.message());
    /**
    * Error processing here is a bit tricky
    *   -   if we are in the middle of processing a messages and
    *       we get an io error with bytes_transfered == 0,
    *       it is probably the other end sending an EOF to signal EOM
    *       so let the parser handle it
    *   -   if this is the first result for a new message and bytes_transfered ==0
    *       this is probably the other end signalling socket closed and a new message
    *       is not being sent. In this case we dont want the parse to see the EOF
    *       as it wont treat it as an error and will keep trying to read.
    *       want to pass it to the callback as EOF
    *   -   otherwise (err && (bytes_transfered > 0)) return with error.
    * For testing purposes we have to differentiate between ending a message
    * with EOF and signalling connection closed with EOF
    */
    if(er && (bytes_transfered > 0)) {
        p_post_message_cb(er);
        LogError("real error condition: ", er.message());
        return;
    }
    // EOF at start of new message - probably client closed connection
    if(er && (bytes_transfered == 0) && (m_total_bytes_read == 0)) {
        p_post_message_cb(er);
        LogError("probably client closed connection: ", er.message());
        return;
    }
    if(er && (bytes_transfered == 0) && (m_total_bytes_read > 0)) {
        LogError("ending message with EOF: ", er.message());
    }
    m_header_buffer_sptr->setSize(bytes_transfered);
//    std::cout << *_header_buffer_sptr << std::endl;
    Marvin::MBuffer& mb = *m_header_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! p_parser_ok(nparsed, mb)) {
        p_post_message_cb(Marvin::make_error_parse());
        return;
    }
    

    if( isFinishedMessage() ) {
        p_post_message_cb(Marvin::make_error_ok());
    } else if( isFinishedHeaders()& (! isFinishedMessage())) {
        if( m_reading_full_message ) {
            p_read_all_body();
        } else {
            p_post_message_cb(Marvin::make_error_ok());
        }
    } else if( ! isFinishedHeaders() ) {
        p_read_some_headers();
    }
}
#pragma mark - methods for reading all body data as part of reading an entire message
/**
* Kicks off the process of reading all body data.
*/
void MessageReader::p_read_all_body()
{
//    _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    p_make_new_body_buffer();
    
    p_read_some_body();
}
/**
* First part of an async loop that reads all body data. This method
* sets up and initiates an async read
*/
void MessageReader::p_read_some_body()
{
    LogDebug(" fd: ", m_read_sock->nativeSocketFD());
    auto h = std::bind(&MessageReader::p_handle_body_read, this, std::placeholders::_1, std::placeholders::_2);
    m_read_sock->asyncRead(m_body_buffer_sptr, h);
}
/**
* Step two of the async loop that reads all body data. This is the async
* completion handler.
*/
void MessageReader::p_handle_body_read(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", m_read_sock->nativeSocketFD());
    /**
    * an io error with bytes_transfered == 0 is probably EOF - let the parser handle it
    * otherwise (err && (bytes_transfered > 0)) return with error
    */
    if(er && (bytes_transfered > 0)) {
        p_post_message_cb(er);
        LogError("", er.message());
    }
    m_body_buffer_sptr->setSize(bytes_transfered);
    Marvin::MBufferSPtr tmp = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(bytes_transfered));
    tmp->append(m_body_buffer_sptr->data(), m_body_buffer_sptr->size());
//    std::cout << std::endl << __FUNCTION__ << ": " << tmp->toString() << std::endl;
    m_raw_body_buffer_chain_sptr->push_back(tmp);
    
    Marvin::MBuffer& mb = *m_body_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! p_parser_ok(nparsed, mb)) {
        p_post_message_cb(Marvin::make_error_parse());
        return;
    }
    
    if( isFinishedMessage()) {
        p_post_message_cb(Marvin::make_error_ok());
    } else {
        p_make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
        p_read_some_body();
    }
}

#pragma mark - methods for reading a chunk of body data and implementing readData
/**
* Called by interface method readBody()
* Initiates the process of reading ONE chunk of body data. Sets up
* a read and completion handler. No looping for this
*/
void MessageReader::p_read_body_chunk()
{
    LogDebug(" fd: ", m_read_sock->nativeSocketFD());
    auto h = std::bind(&MessageReader::p_handle_body_chunk, this, std::placeholders::_1, std::placeholders::_2);
    if(m_body_buffer_sptr == nullptr) {
        p_make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    }
    m_read_sock->asyncRead(m_body_buffer_sptr, h);
}
/**
* Completion handler for reading a single chunk of body data.
*/
void MessageReader::p_handle_body_chunk(Marvin::ErrorType er, std::size_t bytes_transfered)
{
    LogDebug("entry fd: ", m_read_sock->nativeSocketFD());
    /**
    * an io error with bytes_transfered == 0 is probably EOF - let the parser handle it
    * otherwise (err && (bytes_transfered > 0)) return with error
    */
    if(er && (bytes_transfered > 0)) {
        p_post_body_chunk_cb(er, m_body_buffer_chain_sptr);
        LogError("", er.message());
    }
    
    m_body_buffer_sptr->setSize(bytes_transfered);
    Marvin::MBufferSPtr tmp = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(bytes_transfered));
    m_raw_body_buffer_chain_sptr->push_back(tmp);
    
    Marvin::MBuffer& mb = *m_body_buffer_sptr;
    int  nparsed = this->appendBytes((void*)mb.data(), (int)mb.size());
    if( ! p_parser_ok(nparsed, mb)) {
        p_post_body_chunk_cb(Marvin::make_error_parse(), m_body_buffer_chain_sptr);
        return;
    }
    

    if( isFinishedMessage()) {
        p_post_body_chunk_cb(Marvin::make_error_eom(), m_body_buffer_chain_sptr);
        p_make_new_body_buffer_chain();
    } else {
        p_post_body_chunk_cb(Marvin::make_error_ok(), m_body_buffer_chain_sptr);
        p_make_new_body_buffer_chain();
//        _make_new_body_buffer();
//        _body_buffer_sptr = std::shared_ptr<MBuffer>(new MBuffer(_body_buffer_size));
    }
}
#pragma mark - buffer management
/**
* Creates a shared pointer to a single new buffer for reading body data.
*/
void MessageReader::p_make_new_body_buffer()
{
    m_body_buffer_sptr = std::shared_ptr<Marvin::MBuffer>(new Marvin::MBuffer(m_body_buffer_size));
    /// make the MessageBase body buffer refer to the same buffer -
    /// \warning - this needs rethinking
}
void MessageReader::p_make_new_body_buffer_chain()
{
    m_body_buffer_chain_sptr = std::make_shared<Marvin::BufferChain>();
}

#pragma mark - post method for scheduling a callback to run later on the runloop
/**
* Schedules the execution of the _read_message_cb in the future on this instances
* io_service or runloop.
* _read_message_cb is a property that stores the address of the
* callback provided to readMessage() and readHeaders() interface methods.
*/
void MessageReader::p_post_message_cb(Marvin::ErrorType er)
{
    auto pf = std::bind(m_read_message_cb, er);
    m_io.post(pf);
}
/**
* Schedules the execution of the _read_message_cb in the future on this instances
* io_service or runloop.
* _read_body_cb is a property that stores the address of the
* callback provided to readBody();
*/
void MessageReader::p_post_body_chunk_cb(Marvin::ErrorType er, Marvin::BufferChainSPtr chain_sptr)
{
    auto pf = std::bind(m_read_body_cb, er, chain_sptr);
    m_io.post(pf);
}
#pragma mark - error related functions
bool MessageReader::p_parser_ok(int nparsed, Marvin::MBuffer& mb)
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
