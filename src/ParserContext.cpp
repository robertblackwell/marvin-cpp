#include "HTTPMessage.hpp"
#include "http_parser.h"
#include "ParserContext.hpp"

void saveNameValuePair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value)
{

    ParserContext* contextRef = PointerToParserContext(parser->data);
    
    MessageHeaders* headers = &(contextRef->message->headers);
    char* n_p;
    char* v_p;
    int n;
    n = sb_to_string(name, &n_p);
    n = sb_to_string(value,&v_p);
    
    std::string n_str = std::string(n_p);
    std::string v_str = std::string(v_p);
    
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

