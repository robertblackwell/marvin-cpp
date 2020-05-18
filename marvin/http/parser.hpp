
#ifndef marvin_http_parser_hpp
#define marvin_http_parser_hpp

#include <map>
#include <iostream>
#include<http-parser/http_parser.h>
#include <marvin/http/message_base.hpp>
#include <marvin/configure_trog.hpp>

#define kHEADER_STATE_NOTHING 10
#define kHEADER_STATE_FIELD   11
#define kHEADER_STATE_VALUE   12

namespace Marvin {

/// \ingroup http
struct ParserError {
    std::string         name;
    std::string         description;
    enum http_errno     err_number;
};

/**
 *  \ingroup http
 *  \brief An instance of this class will parse a stream of bytes into an http message container.
 *
 *  The data stream is provided to the parser using the "consume" methods. Data can be provided
 *  "all at once" - that is a complete message in one lump or "piece meal" a sequence of arbitarily sized
 *  buffers.
 *
 *  An instance of a parser can parse multiple messages one after the other,
 *  however some internal state needs to be reset between messages and a new container
 *  needs to be provided for the next message.  Both are achieved by a call to Parser::begin()
 *  
 *  It is possible for data from a second message may be present at the end of 
 *  the last buffer from the previous message. The Parser::consume() methods will
 *  return that data without processing after message_done on the first message.
 *  
 *  The caller must retain the returned data (buffer) and present it to 
 *  the a new parser, or after another call to begin() to the same parser,
 *  as the first buffer to be processed for the next message.
 *
 *  It is possible for a "server" to signal the end of a message by simple closing the
 *  connection (rather than include a Content-length header or a Transfer-encoding: chunked header). 
 *  Hence in some cases the parser
 *  will only know that end-of-message has arrived when it is is told.  The way to tell the parse that
 *  the end of data has been hit is to call the method Parser::end(). As a matter of good practice
 *  Parser::end() should be called whever the receiving connection is closed by the sender during
 *  a parsing operation.
 *
 */
class  Parser
{
public:

    using SPtr = std::shared_ptr<Parser>;
    // buffer strategy
    using BodyBufferStrategy = BodyAllocator;
    static std::size_t const HEADER_RESERVE_SIZE = 200;
    Parser();
    ~Parser();
    
    /**
     * \brief  Value returned by parse when processing data.
     */
    enum class ReturnCode {
        error,          /// got a parse error
        end_of_header,  /// encountered enf of header
        end_of_message, /// encountered end of message
        end_of_data     /// processed all the data given
    };
    struct ReturnValue {
        ReturnCode  return_code;
        std::size_t bytes_remaining;
    };

	/**
	 * \brief Send data to the parser for consumption, see details. 
     * 
     * This function will return under one of the following circumstances:
     * 
     * -    it consumed the entire buffer, Indicated by a return value equal to the length parameter.
     * 
     * -    a message was completed. This will be indicated by isFinished() returning true. IN this case
     *      the return value will be < or = length. 
     * 
     *      If return value == length the entire buffer was consumed completing the message.
     * 
     *      If however the return value is < length then some bytes in the buffer (length - return value)
     *      were not processed and are part of the next message.
     * 
     *      The caller should retain the buffer and provide it to a different parser object at the start
     *      of parsing of the next next message.
     * 
     * -    A parsing error is encountered. This will be indicated by isError() returning true. In such a case
     *      getError() will give details of the error. A Parse error is unrecoverable. The connection should
     *      be closed.
     * 
     * -    This function signals completin of parsing of the message header by setting a header complete flag,
     *      which can be tested by a call to header_done()
     * 
     * -    Sometimes a message will be sent that does not contain a content-length nor a chunked-encoding header.
     *      the peer is expecting to signal end of message by closing the connection and as a sign of EOF. The
     *      parser can be informed of this by appending a buffer of zero length or calling appendEOF(). 
     *      The parse may recognize this situation and ask for a terminating eof by setting need_eof tru, this can
     *      be tested by calling needsEOF() after the function returns having consumed all of the buffer.
	 */
	int appendBytes(void* buffer,  unsigned length);

    void begin(MessageBase& msg);
    void begin(MessageBase* msg_ptr);
    /**
     * These functions will consume bytes in the given buffer and return an instance of ReturnValue
     * to indicate the state of the parsing process.
     * There are two processing modes:
     * -    process until a complete message has been parsed
     * -    process until the message header has been completed
     * 
     * To continue and process the body of a message, after having received and processed the header,
     * simply call any of these functions with the default value of the second parameter on 
     * 
     * Note: if a call to a function returns with unprocessed data without end of message
     * being indicated the remaining data in the buffer will need to be passed to consume()
     * before new data can be presented.
     * 
     * Note: if data remains in the buffer when returning from consume() with end_of_message
     * code, that data belongs to the next message and the data needs to be retained
     * and presented as the first buffer once parsing of the new message is underway.
     */
    ReturnValue consume(boost::asio::streambuf& streambuffer, bool only_header = false);
    ReturnValue consume(boost::asio::const_buffer const_buffer, bool only_header = false);
    ReturnValue consume(boost::asio::mutable_buffer mutable_buffer, bool only_header = false);
    ReturnValue consume(const void* buffer, const std::size_t length, const bool only_header = false);
    ReturnValue end();
#if 0
    template <typename B>
    ReturnValue parseMessage(B b)
    {
        consume(b, true)
    }

    template <typename B>
    ReturnValue parseHeader(B b)
    {
        consume(b, false)
    } 
#endif    
	/**
	 * Signal end of data to the parser. Required in some cases as there are message formats that do not
	 * contain message length information.
	 */
	void appendEOF();
    /**
     * Pause and/or unpause the parser
     */
    // void pause();
    // void unPause();
    // bool isPaused() { return( HTTP_PARSER_ERRNO(m_http_parser_ptr) ==  HPE_PAUSED); }
    
    /**
    * Is true if the parser has been given some data to parse.
    * Set false by begin() and true by any consume() 
    */
    bool started;
	/**
	 * is true if parsing of the header fields of the current message is finished.
	 */
	bool header_done;
    
	/**
	 * is true if parsing of current message is finished
	 */
	bool message_done;
    
    bool            isError();
    enum http_errno getErrno();
    ParserError     getError();
    
	/** 
	 * Returns the the Message currently being parsed or just parsed. 
	 */
    MessageBase* currentMessage();
    
    /**
    * C parser class callback functions that interface with the C language parser
    * http-parser from nodejs
    */
    friend int message_begin_cb(http_parser* parser);
    friend int url_data_cb(http_parser* parser, const char* at, size_t length);
    friend int status_data_cb(http_parser* parser, const char* at, size_t length);
    friend int header_field_data_cb(http_parser* parser, const char* at, size_t length);
    friend int header_value_data_cb(http_parser* parser, const char* at, size_t length);
    friend int headers_complete_cb(http_parser* parser);
    friend int chunk_header_cb(http_parser* parser);
    friend int body_data_cb(http_parser* parser, const char* at, size_t length);
    friend int chunk_complete_cb(http_parser* parser);
    friend int message_complete_cb(http_parser* parser);

protected:
    void p_initialize();
    /**
     * member function callbacks  
     */
    int p_on_message_begin(http_parser* parser);
    int p_on_url_data(http_parser* parser, const char* at, size_t length);
    int p_on_status_data(http_parser* parser, const char* at, size_t length);
    int p_on_header_field_data(http_parser* parser, const char* at, size_t length);
    int p_on_header_value_data(http_parser* parser, const char* at, size_t length);
    int p_on_headers_complete(http_parser* parser);
    int p_on_chunk_header(http_parser* parser);
    int p_on_body_data(http_parser* parser, const char* at, size_t length);
    int p_on_chunk_complete(http_parser* parser);
    int p_on_message_complete(http_parser* parser);

    /*
     * These are required to run the parser
     */
    http_parser*             m_http_parser_ptr;
    http_parser_settings*    m_http_parser_settings_ptr;
    MessageBase*             m_current_message_ptr;
    BodyBufferStrategy       m_buffer_strategy;
    ContigBufferFactoryT     m_factory;
    
    ///////////////////////////////////////////////////////////////////////////////////
    //
    // These properties are what used to be parser context object
    // should be private - but for ease of access made public
    //
    ///////////////////////////////////////////////////////////////////////////////////

    int				        header_state;
    ///////////////////////////////////////////////////////////////////////////////////
    // String buffers that interface with the http-parser
    ///////////////////////////////////////////////////////////////////////////////////
    std::string             url_stringbuf;
    std::string             status_stringbuf;
    std::string             name_stringbuf;
    std::string             value_stringbuf;
    ////////////////////////////////////////////////////////////////////
};

} // namespace Marvin
#endif



