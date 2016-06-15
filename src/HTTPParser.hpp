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
 *  The data stream is provided to the parser using one of the appendXXXXX methods. Data can be provided
 *  "all at once" - that is a complete message in one lump or "piece meal" a sequence of arbitarily sized
 *  buffers.
 *
 *  setStreamingOption - configures the parser to parse either a single message or a (continuous) stream
 *  of messages. The default is a single message.
 *
 *  Generally the parser can detect the end of a http messages as most messages formats have
 *  message length information in the message itself. However it is possible for a "server"
 *  to signal the end of a message by simple closing the connection. Hence in some cases the parser
 *  will only know that end-of-message has arrived if it is told.  the way to tell the parse that
 *  the end of data has been hit is to call the method appendEOF.
 *
 *  It is therefore good practice to call appendEOF whenever the code using HTTPPraser is able to detect
 *  an explicit end of data condition. There is no danger in calling appendEOF even when the message
 *  contains length infomation and the parser can self-detect the end of message.
 *
 *  The parser signals progress/outcomes  three "delegate" callbacks .
 *
 *  -   parserDidFinishParsingHeaders(HTTPMessage& aMessage)
 *  -   parserDidFinishParsingMessage(HTTPMessage& aMessage)
 *  -   parserError(NSError anError)
 *
 *  and also via three property/methods
 *
 *  - (BOOL)        onfinishedHeaders:(HTTPMessage*) amessage;
 *  - (BOOL)        onfinishedHeaders:(HTTPMessage*) amessage;
 *  - (NSError*)    onfinishedWithError;
 *
 *  The delegate is optional (that is the parser will not crash is a delegate is not assigned),
 *  and in such a situation the three (finished) method can be used to detect state changes.
 *
 *  NOTE : operating without a delegate, relying on the finish methods, will not work correctly
 *  in a message streaming environment as a data for a subsequent message may/will reset the flags
 *  that the finish methods rely on before they can be used. 
 *
 *  If message streaming is being used a delegate UST be provided and the finish methods NOT relied on.
 *
 * NOTE: messages parsed by this class AE not ready for re-transmission
 * ====================================================================
 *
 *  For example the parser "de-chunks" the body of a message but does NOT remove the "Transfer-Encoding: chunked"
 *  from the list of headers nor does it add a Content-Length header.
 *
 *  The purpose of the parser is NOT to create a message ready for transmission but to decode a message
 *  so that it can be acted upon by a server or client.
 *
 *  If the message is to be re-transmitted the header would need to be "adjusted" to ensure the 
 *  message conforms to the relevant standard and that the receiver
 *  can correctly interpret the message.
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
    void onMessageLambda(std::function<void(HTTPParser*, HTTPMessage*)> cbLambda);
    std::function<void(HTTPParser*, HTTPMessage*)> onMessageLambdaCB;

    void onHeadersLambda(std::function<void(HTTPParser*, HTTPMessage*)> cbLambda);
    std::function<void(HTTPParser*, HTTPMessage*)> onHeadersLambdaCB;

    void onParseErrorLambda(std::function<void(HTTPParser*)> cbLambda);
    std::function<void(HTTPParser*)> onParseErrorLambdaCB;

    void setUpParserCallbacks();
    void setUpNextMessage();
    
	
};
