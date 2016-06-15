//
//  BRNHTTPParser.m
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/17/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//
#include <stdlib.h>
#import "HTTPParser.hpp"
#import "http_parser.h"
#import "simple_buffer.h"
#import "ParserContext.hpp"
#import "HTTPMessage.hpp"

/******************************************************************************/
#pragma mark - forward decleration of parser c-language call back functions

/*
 * A utility function to get the ParserContextRef from the parser object
 */
ParserContext*  parserContextFromParser(http_parser* parser );

int message_begin_cb(http_parser* parser);
int url_data_cb(http_parser* parser, const char* at, size_t length);
int status_data_cb(http_parser* parser, const char* at, size_t length);
int header_field_data_cb(http_parser* parser, const char* at, size_t length);
int header_value_data_cb(http_parser* parser, const char* at, size_t length);
int headers_complete_cb(http_parser* parser);
int body_data_cb(http_parser* parser, const char* at, size_t length);
int message_complete_cb(http_parser* parser);

/******************************************************************************/


HTTPParser::HTTPParser()
{
    last_message = NULL;
    messageCompleteFlag = false;
    headersCompleteFlag = false;
    streamMessagesOption = false;
    setUpParserCallbacks();
}

HTTPParser::~HTTPParser()
{
    free(parser);
    free(parserSettings);
    parser = NULL;
    parserSettings = NULL;
    contextRef = NULL;
}

void HTTPParser::setStreamingOption(bool streamingOption)
{
    streamMessagesOption = streamingOption;
}

int HTTPParser::appendBytes(void *buffer, unsigned length)
{
    messageData.append((char*)buffer, length);
    size_t nparsed = http_parser_execute(parser, parserSettings, (char*)buffer, (int)length);
    return nparsed;
}

void HTTPParser::appendEOF()
{
    char* buffer = NULL;
    int someLength = 0;
    if( ! messageCompleteFlag )
    {
        size_t nparsed = http_parser_execute(parser, parserSettings, buffer, someLength);
    }
}


bool HTTPParser::finishedHeaders()
{
    return headersCompleteFlag;
}

bool HTTPParser::finishedMessage()
{
    return messageCompleteFlag;
}

HTTPMessage* HTTPParser::currentMessage()
{
    return last_message;
}

#pragma mark - private methods

void HTTPParser::setUpNextMessage()
{
    if( streamMessagesOption )
    {
        messageCompleteFlag = false;
        headersCompleteFlag = false;
        contextRef->message = new HTTPMessage();
        
        messageData = "";
        
        if(contextRef->url_buf != NULL){
            sb_free(contextRef->url_buf);
            contextRef->url_buf = NULL;
        }
        if( contextRef->status_buf != NULL){
            sb_free(contextRef->status_buf);
            contextRef->status_buf = NULL;
        }
        if(contextRef->name != NULL){
            sb_free(contextRef->name);
            contextRef->name = NULL;
        }
        if(contextRef->value != NULL){
            sb_free(contextRef->value);
            contextRef->value = NULL;
        }
    }
}

void HTTPParser::setUpParserCallbacks()
{
    parser = (http_parser*)malloc(sizeof(http_parser));
    
    http_parser_init( parser, HTTP_BOTH );
    contextRef = new ParserContext();
    
    contextRef->objc_parser = this;
    contextRef->message = new HTTPMessage();
    
    context_pointer = ParserContextRefToPointer(contextRef);
    parser->data = context_pointer;
    
    http_parser_settings* settings = (http_parser_settings*)malloc(sizeof(http_parser_settings));
    parserSettings = settings;
    
    /* Now set up the call back functions */
    
    settings->on_message_begin = message_begin_cb;
    settings->on_url = url_data_cb;
    settings->on_status = status_data_cb;
    
    settings->on_header_field = header_field_data_cb;
    settings->on_header_value = header_value_data_cb;
    
    settings->on_headers_complete = headers_complete_cb;
    
    settings->on_body = body_data_cb;
    settings->on_message_complete = message_complete_cb;
    
}


#pragma mark - c-language call back functions implementation

ParserContext*  parserContextFromParser(http_parser* parser )
{
    ParserContext* t = (ParserContext*)(parser->data);
    
    return t;
}


int
message_begin_cb(http_parser* parser)
{
    ParserContext* context = parserContextFromParser(parser);
    HTTPParser* p = (HTTPParser*) context->objc_parser;

    return 0;
}

int
url_data_cb(http_parser* parser, const char* at, size_t length)
{
    ParserContext* context = parserContextFromParser(parser);
    
    if( context->url_buf == NULL)
        context->url_buf = sb_create();
    
    sb_append( context->url_buf, (char*)at, length);
    return 0;
}

int
status_data_cb(http_parser* parser, const char* at, size_t length)
{
    ParserContext* context = parserContextFromParser(parser);
    
    if( context->status_buf == NULL)
        context->status_buf = sb_create();
    sb_append( context->status_buf, (char*)at, length);
    return 0;
}

int
header_field_data_cb(http_parser* parser, const char* at, size_t length)
{
    ParserContext* c = parserContextFromParser(parser);
    
    
    int state = c->header_state;

    if( (state == 0)||(state == kHEADER_STATE_NOTHING) || (state == kHEADER_STATE_VALUE)){
        if( c->name != NULL){
            saveNameValuePair(parser, c->name, c->value);
            sb_free(c->name); c->name = NULL;
            sb_free(c->value);c->value = NULL;
        }
        simple_buffer_t* sb = sb_create();
        sb_append(sb, (char*)at, length);
        c->name = sb;
        c->value = NULL;
    } else if( state == kHEADER_STATE_FIELD){
        simple_buffer_t* sb = c->name;
        sb_append(sb, (char*)at, length);
    }
    c->header_state = kHEADER_STATE_FIELD;
    
    return 0;
}
int
header_value_data_cb(http_parser* parser, const char* at, size_t length)
{
    ParserContext* c = parserContextFromParser(parser);
    
    int state = c->header_state;
    
    if( state == kHEADER_STATE_FIELD ){
        simple_buffer_t* sb = sb_create();
        sb_append(sb, (char*)at, length);
        c->value = sb;
    } else if( state == kHEADER_STATE_VALUE){
        simple_buffer_t* sb = c->value;
        sb_append(sb, (char*)at, length);
    }
    c->header_state = kHEADER_STATE_VALUE;
    return 0;
}

int
headers_complete_cb(http_parser* parser)
{
    ParserContext* c = parserContextFromParser(parser);
    
    if( c->name != NULL){
        saveNameValuePair(parser, c->name, c->value);
    }
    
    c->message->status_code = parser->status_code;
    c->message->http_major = parser->http_major;
    c->message->http_minor = parser->http_minor;
    if( c->url_buf == NULL ){
        c->message->url = "";
    }else{
        c->message->url = std::string(c->url_buf->buffer, c->url_buf->used);
    }
    if( c->status_buf == NULL ){
        c->message->status = "";
    }else{
        c->message->status = std::string(c->status_buf->buffer, c->status_buf->used);
    }
    
    c->message->method = static_cast<http_method>(parser->method);

    HTTPParser* p = (HTTPParser*) c->objc_parser;
    
    p->headersCompleteFlag = true;
    p->last_message = c->message;

    return 0;
}

int
body_data_cb(http_parser* parser, const char* at, size_t length)
{
    ParserContext* c = parserContextFromParser(parser);
    
//    c->body->appendBytes(at, length);
    (c->message->body)->append(at, length);
    return 0;
}

int
message_complete_cb(http_parser* parser)
{
    ParserContext* c = parserContextFromParser(parser);
    
//    c->message->body = c->body;
    
    HTTPParser* p = (HTTPParser*) (c->objc_parser);
    
    p->last_message = c->message;
    p->messageCompleteFlag = true;

    /*
     * Now get ready for the next message
     */
    p->setUpNextMessage();
    
    return 0;
}

