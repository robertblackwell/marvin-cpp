#include "HTTPMessage.hpp"
#include "HTTPParser.hpp"
#include "http_parser.h"
#include "ParserContext.hpp"

ParserContext::ParserContext()
{
    this->name = NULL;
    this->value = NULL;
    this->status_buf = NULL;
    this->url_buf = NULL;
    this->header_state = kHEADER_STATE_NOTHING;
    this->message = new HTTPMessage();
}
ParserContext::~ParserContext()
{
    
}

void saveNameValuePair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value)
{
    HTTPParser* p = getHTTPParser(parser);
    
    ParserContext* contextRef = getParserContext(parser);
        
    MessageHeaders* headers = &(contextRef->message->headers);
    char* n_p;
    char* v_p;
    int n;
    n = sb_to_string(name, &n_p);
    n = sb_to_string(value,&v_p);
    
    std::string n_str = std::string(name->buffer, name->used);
    std::string v_str = std::string(value->buffer, value->used);
    
    free(v_p);
    free(n_p);
    
    (*headers)[n_str] = v_str;
    
}

ParserContext*  PointerToParserContext(void* pointer)
{
    ParserContext*  ref = (ParserContext *)((void*) pointer);
    return ref;
}

void*   ParserContextRefToPointer(ParserContext* ref)
{
    void* pointer = (void*) ref;
    return pointer;
}

