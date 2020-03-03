
#ifndef marvin_http_parser_hpp
#define marvin_http_parser_hpp

#include <map>
#include <iostream>
#include <marvin/external_src/http-parser/http_parser.h>
#include <marvin/external_src/simple_buffer/simple_buffer.h>
#include<marvin/http/message_base.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>

#define kHEADER_STATE_NOTHING 10
#define kHEADER_STATE_FIELD   11
#define kHEADER_STATE_VALUE   12

namespace Marvin {
namespace Http {

/// \ingroup HttpMessage
struct ParserError {
    std::string         name;
    std::string         description;
    enum http_errno     err_number;
};

/**
* \ingroup HttpMessage
* \brief This ABSTRACT class parses streams of data into http Message objects 
 *  (or at least the first line + headers - message body is a little more complicated).
 *
 *  This class is ABSTRACT and the expectation is that this class will be used as a base class
 *  for a more complicated object such as an incoming Request object or an incoming Response object.
 *
 *  In homage to nodejs we could have used the name IncomingMessage - maybe we will in the future
 *
 *  The data stream is provided to the parser using the appendBytes method. Data can be provided
 *  "all at once" - that is a complete message in one lump or "piece meal" a sequence of arbitarily sized
 *  buffers.
 *
 *  setStreamingOption - configures the parser to parse either a single message or a (continuous) stream
 *  of messages. The default is a single message. STREAMING has not been tested
 *
 *  Generally the parser can detect the end of a http messages as most messages formats have
 *  message length information in the message itself, or each "chunk" has a chunk length. 
 *
 *  However it is possible for a "server"
 *  to signal the end of a message by simple closing the connection. Hence in some cases the parser
 *  will only know that end-of-message has arrived when it is is told.  The way to tell the parse that
 *  the end of data has been hit is to call the method appendEOF.
 *
 *  It is therefore good practice to call appendEOF whenever the code using HTTPParser is able to detect
 *  an explicit end of data condition. There is no danger in calling appendEOF even when the message
 *  contains length infomation and the parser can self-detect the end of message.
 *
 *  A call to parser.appendBytes will only return when:
 *
 *  -   it has processed all the bytes that it was given, or
 *	-	the message had an UPGRADE method,
 *  -   on of the virtual call back overrides returns 1 to pause the parser
 *  -   it encountered a parse error
 *
 *  - IT DOES NOT RETURN ON COMPLETION OF PARSING A FULL MESSAGE
 *  BUT WE HAVE FORCE THIS BY HAVING OUR MESSAGE COMPLETE CALLBACK
 *  EXPLICITLY PAUSE THE PARSER.
 *
 *  The parser as we use it will not support reading multiple messages from the
 *  the same buffer. Each message on a connection requires an new Parser.
 *
 *  The following "callbacks" are provided in the form of virtual methods - NOT PURE
 *  and they MAY be overridden by a derived class to capture the corresponding event
 *
 *          virtual void OnParseBegin()
 *          virtual void OnHeadersComplete(MessageInterface* msg)
 *          virtual void OnMessageComplete(MessageInterface* msg)
 *          virtual void OnParseError()
 *          virtual void OnBodyData(void* buf, int len)
 *          virtual void OnChunkBegin(int chunkLength)
 *          virtual void OnChunkData(void* buf, int len)
 *          virtual void OnChunkEnd()
 *
 *  The OnMessageComplete and OnHeadersComplete methods may terminate parsing by returning "true" or 1.
 *
 * WARNING - DO NOT PERFORM ANY IO OR RELINGUISH THE RUNLOOP/IO_SERVICE INSIDE THESE CALLBACKS
 *
 * PURE VIRTUAL Abstract class
 *
 *  Derived class must implement pure virual method
 *
 *          MessageInterface* currentMessage()
 */
class  Parser
{
public:

    
    Parser();
    ~Parser();
    
	/**
	 * Sets the message streaming option - to the value of streamOption
	 */
	void setStreamingOption(bool  streamingOption);

	/**
	 * send data to the parser for consumption
	 */
	int appendBytes(void* buffer,  unsigned length);

	/**
	 * Signal end of data to the parser. Required in some cases as there are message formats that do not
	 * contain message length information.
	 */
	void appendEOF();

    /**
     * Pause and/or unpause the parser
     */
    void pause();
    void unPause();
    bool isPaused() { return( HTTP_PARSER_ERRNO(parser) ==  HPE_PAUSED); }
    
	/**
	 * return true if parsing of the header fields of the current message is finished.
	 * In which case the header fields can be obtained thru the headers property of
	 * the HTTPMessage returned from * getCurrentMessage
	 */
	bool isFinishedHeaders();
    
	/**
	 * return true if parsing of current message is finished, in which case the message
	 * can be obtained via the getCurrentMessage method
	 */
	bool isFinishedMessage();
    
    bool            isError();
    enum http_errno getErrno();
    ParserError     getError();
    
	/** 
	 * Returns the the Message currently being parsed. When operating in non streaming mode
	 * this also returns the most recently parsed message.
	 */
    virtual MessageInterface* currentMessage() = 0;
	
    
    /*
     * Three "call backs" for 
     *  -   headers complete
     *  -   message complete
     *  -   body data after de-chunking
     *  -   parse error
     *
     */
    
    virtual void OnParseBegin();
    virtual void OnHeadersComplete(MessageInterface* msg, void* body_start_ptr, std::size_t remainder);
    virtual void OnMessageComplete(MessageInterface* msg);
    virtual void OnParseError();
    virtual void OnBodyData(void* buf, int len);
    virtual void OnChunkBegin(int chunkLength);
    virtual void OnChunkData(void* buf, int len);
    virtual void OnChunkEnd();
    
    void setUpParserCallbacks();
    void setUpNextMessage();
    
    
    //
    // Utility functions that interface the C parser with the C++ Parser class
    //
    friend void saveNameValuePair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value);
    friend Parser* getParser(http_parser* parser);
    friend void setParser(http_parser* c_parser, Parser* cpp_parser);
    
    //
    // C parser class back functions
    //
    friend int message_begin_cb(http_parser* parser);

    friend int url_data_cb(http_parser* parser, const char* at, size_t length);
    friend int status_data_cb(http_parser* parser, const char* at, size_t length);
    friend int header_field_data_cb(http_parser* parser, const char* at, size_t length);
    friend int header_value_data_cb(http_parser* parser, const char* at, size_t length);
    friend int headers_complete_cb(http_parser* parser, const char* aptr, size_t remainder);

    friend int chunk_size_start(http_parser* parser, const char* at, size_t length);
    friend int chunk_header_cb(http_parser* parser);
    friend int body_data_cb(http_parser* parser, const char* at, size_t length);
    friend int chunk_complete_cb(http_parser* parser);
    friend int message_complete_cb(http_parser* parser);

protected:
    /*
     * These are required to run the parser
     */
    http_parser*            parser;
    http_parser_settings*   parserSettings;
    bool                    headersCompleteFlag;
    bool                    messageCompleteFlag;
    std::string             messageData;
    
    ///////////////////////////////////////////////////////////////////////////////////
    //
    // These properties are what used to be parser context object
    // should be private - but for ease of access made public
    //
    ///////////////////////////////////////////////////////////////////////////////////

    int				header_state;
    ///////////////////////////////////////////////////////////////////////////////////
    // These are C structures and must be allocated and freed explicitly with SBFree()
    // They are also temporary working variables used only during the parsing process
    ///////////////////////////////////////////////////////////////////////////////////
    simple_buffer_t*   url_buf;
    simple_buffer_t*   status_buf;
    simple_buffer_t*   name_buf;
    simple_buffer_t*   value_buf;
    Marvin::Http::Headers    headers;
    ////////////////////////////////////////////////////////////////////
    
    
	
};

} // namespave Http
} // namespace Marvin
#endif



