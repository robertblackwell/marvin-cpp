//
//  BRNHTTPParser.h
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/17/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//

#include "http_parser.h"
#include "ParserContext.hpp"


/*
 *  This class parses streams of data into streams of HTTPMessage objects.
 *
 *  The data stream is provided to the parser using the appendBytes method. Data can be provided
 *  "all at once" - that is a complete message in one lump or "piece meal" a sequence of arbitarily sized
 *  buffers.
 *
 *  setStreamingOption - configures the parser to parse either a single message or a (continuous) stream
 *  of messages. The default is a single message.
 *
 *  Generally the parser can detect the end of a http messages as most messages formats have
 *  message length information in the message itself, or each "chunk" has a chunk length. 
 *
 *  However it is possible for a "server"
 *  to signal the end of a message by simple closing the connection. Hence in some cases the parser
 *  will only know that end-of-message has arrived unless it is told.  The way to tell the parse that
 *  the end of data has been hit is to call the method appendEOF.
 *
 *  It is therefore good practice to call appendEOF whenever the code using HTTPPraser is able to detect
 *  an explicit end of data condition. There is no danger in calling appendEOF even when the message
 *  contains length infomation and the parser can self-detect the end of message.
 *
 *  A call to parser.appendBytes will only return when:
 *
 *  -   it has processed all the bytes that it was given, or
 *  -   it encountered a parse error
 *
 *  - IT DOES NOT RETURN ON COMPLETION OF PARSING A FULL MESSAGE
 *
 *  In order to get a full parsed message the onMessagecB  .. see below ..must be provided.
 *
 *  This is important in a situation where multiple messages may be available at the input socket.
 *  Since the parser would move on to the next message without returning.
 *
 *  Three callbacks are provided, these are instances of cpp std::function objects, onMessageCB
 *  is required (manditory) the other two optional.
 *
 *  -   onHeadersCB
 *  -   onMessageCB
 *  -   onParserErrorCB
 *
 *  THe onMessageCB interface provides a facility for the call back to terminate the parsers processing. It
 *  does this via a call backs bool breakFlag parameter. This is provided to support proxying the CONNECT
 *  request where only one HTTP messages is required (and only one wanted) from the client. This enables the
 *  call back to terminate parsing of any other data on the socket after the first full message is parsed.
 *
 *  When terminated in this way a call to parser.appendBytes may return a value that is less than the number of bytes
 *  given it. This situation would normally signify a parse error. The application must maintain other state
 *  information (such as a break flag) to distinguish a forced terminate from a parsing error.
 *
 *
 */

class HTTPParser;


HTTPParser* getHTTPParser(http_parser* parser);

ParserContext* getParserContext(http_parser* parser);

void setHTTPParser(http_parser* c_parser, HTTPParser* cpp_parser);


typedef void(*HTTPParserCallBackFunction)(HTTPParser* parser);

class  HTTPParser
{
public:
	bool                    streamMessagesOption;
	bool                    headersCompleteFlag;
	bool                    messageCompleteFlag;
    std::string             messageData;

	/*
	 * These are required to run the parser
	 */
	http_parser*            parser;
	void*                   context_pointer;
	http_parser_settings*   parserSettings;

    ParserContext           parserContext;
	
	HTTPMessage*            last_message;
	
	void*					onFinishedHeaders;
	void*					onFinishedMessage;
	void*					onError;

    
    HTTPParser();
    ~HTTPParser();
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
	 * return true if parsing of the header fields of the current message is finished.
	 * In which case the header fields can be obtained thru the headers property of
	 * the HTTPMessage returned from * getCurrentMessage
	 */
	bool finishedHeaders();
    
	/**
	 * return true if parsing of current message is finished, in which case the message
	 * can be obtained via the getCurrentMessage method
	 */
	bool finishedMessage();

	/** 
	 * Returns the the HTTPMessage currently being parsed. When operating in non streaming mode
	 * this also returns the most recently parsed message.
	 *
	 * In streaming mode this also returns the most "recently parsed message" but that is an ill-defined
	 * concept in the context of streaming. In streaming mode rely on the delegate methods.
	 */
	HTTPMessage* currentMessage();
	
    
    /*
     * Three "call backs" for 
     *  -   headers complete
     *  -   message complete
     *  -   parse error
     *
     */
    void onMessageLambda(std::function<void(HTTPParser*, HTTPMessage*, bool*)> cbLambda);
    std::function<void(HTTPParser*, HTTPMessage*, bool*)> onMessageLambdaCB;

    void onHeadersLambda(std::function<void(HTTPParser*, HTTPMessage*)> cbLambda);
    std::function<void(HTTPParser*, HTTPMessage*)> onHeadersLambdaCB;

    void onParseErrorLambda(std::function<void(HTTPParser*)> cbLambda);
    std::function<void(HTTPParser*)> onParseErrorLambdaCB;

    void setUpParserCallbacks();
    void setUpNextMessage();
    
	
};
