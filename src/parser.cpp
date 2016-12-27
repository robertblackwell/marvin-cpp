//
//  BRNParser.m
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/17/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include "http_header.hpp"

#include "rb_logger.hpp"
#include "parser.hpp"


RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

/******************************************************************************/
#pragma mark - forward decleration of parser c-language call back functions

/*
 * A utility function to get the ParserContextRef from the parser object
 */
//ParserContext*  parserContextFromParser(http_parser* parser );

int chunk_header_cb(http_parser* parser);
int chunk_complete_cb(http_parser* parser);

int message_begin_cb(http_parser* parser);
int url_data_cb(http_parser* parser, const char* at, size_t length);
int status_data_cb(http_parser* parser, const char* at, size_t length);
int header_field_data_cb(http_parser* parser, const char* at, size_t length);
int header_value_data_cb(http_parser* parser, const char* at, size_t length);
int headers_complete_cb(http_parser* parser);
int body_data_cb(http_parser* parser, const char* at, size_t length);
int message_complete_cb(http_parser* parser);

/******************************************************************************/


Parser::Parser()
{
    messageCompleteFlag = false;
    headersCompleteFlag = false;
    messageData = "";
    
    url_buf = NULL;
    status_buf = NULL;
    name_buf = NULL;
    value_buf = NULL;
    header_state = kHEADER_STATE_NOTHING;

    setUpParserCallbacks();
}

Parser::~Parser()
{
    free(parser);
    free(parserSettings);
    parser = NULL;
    parserSettings = NULL;
}

void Parser::pause()
{
    http_parser_pause(parser, 1);
}
void Parser::unPause()
{
    http_parser_pause(parser, 0);
}

int Parser::appendBytes(void *buffer, unsigned length)
{
    messageData.append((char*)buffer, length);
    size_t nparsed = http_parser_execute(parser, parserSettings, (char*)buffer, (int)length);
    return (int)nparsed;
}

void Parser::appendEOF()
{
    char* buffer = NULL;
    size_t nparsed;
    int someLength = 0;
    if( ! messageCompleteFlag )
    {
        nparsed = http_parser_execute(parser, parserSettings, buffer, someLength);
    }
    LogDebug("back from parser nparsed: ", nparsed);
}


bool Parser::isFinishedHeaders()
{
    return headersCompleteFlag;
}

bool Parser::isFinishedMessage()
{
    return messageCompleteFlag;
}

enum http_errno Parser::getErrno()
{
    return (enum http_errno) this->parser->http_errno;
}

bool Parser::isError(){
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    enum http_errno x = (enum http_errno)this->parser->http_errno;
    char* n = (char*)http_errno_name(x);
    char* d = (char*)http_errno_description(x);
#pragma clang diagnostic pops
    FLogDebug(" errno: %d name: %s, description: %s", this->parser->http_errno, n,d);
    return (this->parser->http_errno != 0);
};

void Parser::OnParseBegin()
{
    LogVerbose("");
};
void Parser::OnHeadersComplete(MessageInterface* msg)
{
    // This is the virtual method in parser.hpp
    headersCompleteFlag = true;
    LogVerbose("");
};
void Parser::OnMessageComplete(MessageInterface* msg)
{
    LogVerbose("");
};
void Parser::OnParseError()
{
    LogVerbose("");
};
void Parser::OnBodyData(void* buf, int len)
{
    LogVerbose("");
};
void Parser::OnChunkBegin(int chunkLength)
{
    LogVerbose("");
};
void Parser::OnChunkData(void* buf, int len)
{
    LogVerbose("");
};
void Parser::OnChunkEnd()
{
    LogVerbose("");
};


/*
 **************************************************************************************************
 */

#pragma mark - private methods

void Parser::setUpNextMessage()
{
    messageCompleteFlag = false;
    headersCompleteFlag = false;
    
    messageData = "";
    
    if(url_buf != NULL){
        sb_free(url_buf);
        url_buf = NULL;
    }
    if( status_buf != NULL){
        sb_free(status_buf);
        status_buf = NULL;
    }
    if(name_buf != NULL){
        sb_free(name_buf);
        name_buf = NULL;
    }
    if(value_buf != NULL){
        sb_free(value_buf);
        value_buf = NULL;
    }
}

void Parser::setUpParserCallbacks()
{
    parser = (http_parser*)malloc(sizeof(http_parser));
    
    http_parser_init( parser, HTTP_BOTH );
    
    setParser(parser, this);
    
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
    settings->on_chunk_header = chunk_header_cb;
    settings->on_chunk_complete = chunk_complete_cb;
    
}


#pragma mark - c-language call back functions implementation
Parser* getParser(http_parser* parser)
{
    return (Parser*) parser->data;
}


void saveNameValuePair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value)
{
    Parser* p = getParser(parser);
    char* n_p;
    char* v_p;
    int n;
    n = sb_to_string(name, &n_p);
    n = sb_to_string(value,&v_p);
    
    HttpHeader::canonicalKey(name->buffer, name->used);
    
    std::string n_str = std::string(name->buffer, name->used);
    std::string v_str = std::string(value->buffer, value->used);
    
    free(v_p);
    free(n_p);
    
    (p->headers)[n_str] = v_str;
    MessageInterface* m = p->currentMessage();
    m->setHeader(n_str, v_str);
//    std::cout << "SaveNameValuePair::set " << n_str << " " << v_str << std::endl;
//    auto h = p->headers;
//    for (auto iter = h.begin(); iter != h.end(); iter++)
//    {
//        std::cout << "SaveNameValuePair: "<< "Key: " << iter->first << " : " << iter->second << std::endl;
//    }
    
}


void setParser(http_parser* c_parser, Parser* cpp_parser)
{
    c_parser->data = (void*) cpp_parser;
}

int
message_begin_cb(http_parser* parser)
{
    Parser* p =  getParser(parser);
    p->OnParseBegin();
    return 0;
}

int
url_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p = getParser(parser);
    MessageInterface* message = p->currentMessage();

    if( p->url_buf == NULL)
        p->url_buf = sb_create();
        
    message->setIsRequest(true);
    sb_append( p->url_buf, (char*)at, length);
    return 0;
}

int
status_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p = getParser(parser);
    MessageInterface* message = p->currentMessage();
    if( p->status_buf == NULL)
        p->status_buf = sb_create();
    
    message->setIsRequest(false);
    sb_append( p->status_buf, (char*)at, length);
    return 0;
}

int
header_field_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p = getParser(parser);
    int state = p->header_state;
    if( (state == 0)||(state == kHEADER_STATE_NOTHING) || (state == kHEADER_STATE_VALUE)){
        if( p->name_buf!= NULL){
            saveNameValuePair(parser, p->name_buf, p->value_buf);
            sb_free(p->name_buf); p->name_buf= NULL;
            sb_free(p->value_buf);p->value_buf = NULL;
        }
        simple_buffer_t* sb = sb_create();
        sb_append(sb, (char*)at, length);
        p->name_buf= sb;
        p->value_buf = NULL;
    } else if( state == kHEADER_STATE_FIELD){
        simple_buffer_t* sb = p->name_buf;
        sb_append(sb, (char*)at, length);
    }else{
        assert(false);
    }
    p->header_state = kHEADER_STATE_FIELD;
    return 0;
}
int
header_value_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p = getParser(parser);
    int state = p->header_state;
    
    if( state == kHEADER_STATE_FIELD ){
        simple_buffer_t* sb = sb_create();
        sb_append(sb, (char*)at, length);
        p->value_buf = sb;
    } else if( state == kHEADER_STATE_VALUE){
        simple_buffer_t* sb = p->value_buf;
        sb_append(sb, (char*)at, length);
    } else{
        assert(false);
    }
    p->header_state = kHEADER_STATE_VALUE;
    return 0;
}

int
headers_complete_cb(http_parser* parser)
{
    Parser* p = getParser(parser);

    MessageInterface* message = p->currentMessage();
    
    if( p->name_buf != NULL){
        saveNameValuePair(parser, p->name_buf, p->value_buf);
    }
    message->setMethod((enum http_method)parser->method);
    message->setStatusCode( parser->status_code );
    message->setHttpVersMajor( parser->http_major );
    message->setHttpVersMinor( parser->http_minor );
    if( p->url_buf == NULL ){
        message->setUri( "" );
    }else{
        message->setUri( std::string(p->url_buf->buffer, p->url_buf->used) );
    }
    if( p->status_buf == NULL ){
        message->setStatus(std::string(""));
    }else{
        message->setStatus( std::string(p->status_buf->buffer, p->status_buf->used) );
    }
    
    p->headersCompleteFlag = true;
    p->OnHeadersComplete(message);
    return 0;
}

int
body_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p = getParser(parser);
    
//    (p->message->body)->append(at, length);

    p->OnBodyData((void*)at, (int)length);
    p->OnChunkData((void*)at, (int)length);
    return 0;

}
int chunk_header_cb(http_parser* parser)
{
    Parser* p = getParser(parser);
    p->OnChunkBegin((int)parser->content_length);
    return 0;
}

int chunk_complete_cb(http_parser* parser)
{
    Parser* p = getParser(parser);
    p->OnChunkEnd();
    return 0;
}


int
message_complete_cb(http_parser* parser)
{
    Parser* p = getParser(parser);
    p->messageCompleteFlag = true;
    
    MessageInterface* message = p->currentMessage();
    p->OnMessageComplete(message);
    /*
     * Now get ready for the next message
     */
    return 0;
}

