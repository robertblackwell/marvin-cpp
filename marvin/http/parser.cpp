#include <marvin/http/parser.hpp>

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <marvin/http/headers_v2.hpp>
#include <marvin/exception.hpp>
#include <marvin/macros.hpp>
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

using namespace Marvin;

namespace Marvin {

/// forward declares 
int chunk_header_cb(http_parser* parser);
int chunk_complete_cb(http_parser* parser);
int message_begin_cb(http_parser* parser);
int url_data_cb(http_parser* parser, const char* at, size_t length);
int status_data_cb(http_parser* parser, const char* at, size_t length);
int header_field_data_cb(http_parser* parser, const char* at, size_t length);
int header_value_data_cb(http_parser* parser, const char* at, size_t length);
int headers_complete_cb(http_parser* parser);//, const char* aptr, size_t remainder);
int body_data_cb(http_parser* parser, const char* at, size_t length);
int message_complete_cb(http_parser* parser);

Parser::Parser(MessageBase* current_message_ptr): m_current_message_ptr(current_message_ptr)
{
    message_done = false;
    header_done = false;
    
    url_buf = NULL;
    status_buf = NULL;
    name_buf = NULL;
    value_buf = NULL;
    header_state = kHEADER_STATE_NOTHING;
    p_setup_callbacks();
}

Parser::~Parser()
{
    free(m_http_parser_ptr);
    free(m_http_parser_settings_ptr);
    m_http_parser_ptr = NULL;
    m_http_parser_settings_ptr = NULL;
    if (url_buf != NULL) sb_free(url_buf);
    if (status_buf != NULL) sb_free(status_buf);
    if (name_buf != NULL) sb_free(name_buf);
    if (value_buf != NULL) sb_free(value_buf);
}

MessageBase* Parser::currentMessage()
{
    return m_current_message_ptr;
}
// void Parser::pause()
// {
//     http_parser_pause(m_http_parser_ptr, 1);
// }
// void Parser::unPause()
// {
//     http_parser_pause(m_http_parser_ptr, 0);
// }

int Parser::appendBytes(void *buffer, unsigned length)
{
    std::string tmp = std::string((char*)buffer, length);
    size_t nparsed = http_parser_execute(m_http_parser_ptr, m_http_parser_settings_ptr, (char*)buffer, (int)length);
    return (int)nparsed;
}
void Parser::begin(MessageBase& message_ref)
{
    this->m_current_message_ptr = std::addressof<MessageBase>(message_ref);
}
void Parser::begin(MessageBase* message_ptr)
{
    this->m_current_message_ptr = message_ptr;
}
Parser::ReturnValue Parser::consume(boost::asio::streambuf& streambuffer, bool only_header)
{
    namespace ba = boost::asio;
    ReturnValue rv{};
    while (streambuffer.data().size() > 0) {
        ba::const_buffer const_b = streambuffer.data();
        rv = consume(const_b, only_header);
        streambuffer.consume(const_b.size() - rv.bytes_remaining);
        switch (rv.return_code) {
            case ReturnCode::error:
            case ReturnCode::end_of_message:
                return rv;
                break;
            case ReturnCode::end_of_header:
                if (only_header) {
                    return rv;
                }
                break;
            case ReturnCode::end_of_data:
            break;
        }
    }
    return rv;

}
Parser::ReturnValue Parser::consume(boost::asio::const_buffer const_buffer, bool only_header)
{
    ReturnValue rv = consume(const_buffer.data(), const_buffer.size(), only_header);
    return rv;
}
Parser::ReturnValue Parser::consume(boost::asio::mutable_buffer mutable_buffer, bool only_header)
{
    ReturnValue rv = consume(mutable_buffer.data(), mutable_buffer.size(), only_header);
    return rv;
}

Parser::ReturnValue Parser::consume(const void* buf, std::size_t length, bool only_header)
{
    ReturnValue rv{.return_code = ReturnCode::end_of_data, .bytes_remaining = length};
    char* b = (char*) buf;
    // if (length == 0) {
    //     this->appendEOF();
    //     if (this->isError()) {
    //         rv.return_code = ReturnCode::error;
    //         auto x = getError();
    //         return rv;
    //     } else if (this->message_done) {
    //         rv.return_code = ReturnCode::end_of_message;
    //         return rv;
    //     } else if (this->header_done) {
    //         rv.return_code = ReturnCode::end_of_header;
    //         if (only_header) {
    //             return rv;
    //         }
    //     } else {
    //         std::cout << "should not be here" << std::endl;
    //     return rv;
    //     }
    // }
    std::size_t total_parsed = 0;
    while (total_parsed < length) {
        char* b_start_ptr = &(b[total_parsed]);
        int nparsed = this->appendBytes((void*) b_start_ptr, length - total_parsed);
        total_parsed = total_parsed + nparsed;
        std::cout << "nparsed: " << nparsed  << "len: " << length - total_parsed << " content: " << buf <<  std::endl;
        rv.bytes_remaining = length - nparsed;
        if (this->isError()) {
            rv.return_code = ReturnCode::error;
            auto x = getError();
            return rv;
        } else if (this->message_done) {
            rv.return_code = ReturnCode::end_of_message;
            return rv;
        } else if (this->header_done) {
            rv.return_code = ReturnCode::end_of_header;
            if (only_header) {
                return rv;
            }
        } else if (nparsed == length) {

        }
    }
    return rv;
}
Parser::ReturnValue Parser::end()
{
    ReturnValue rv{.return_code = ReturnCode::end_of_data, .bytes_remaining = 0};
    char* buffer = NULL;
    size_t nparsed;
    int someLength = 0;
    if( ! message_done ) {
        nparsed = http_parser_execute(m_http_parser_ptr, m_http_parser_settings_ptr, buffer, someLength);
        if (this->isError()) {
            rv.return_code = ReturnCode::error;
            auto x = getError();
            return rv;
        } else if (this->message_done) {
            rv.return_code = ReturnCode::end_of_message;
            return rv;
        } else if (this->header_done) {
            rv.return_code = ReturnCode::end_of_header;
        } else {
            std::cout << "should not be here" << std::endl;
            MARVIN_THROW("dont think we should get here");
            return rv;
        }
    }
}
void Parser::appendEOF()
{
    char* buffer = NULL;
    size_t nparsed;
    int someLength = 0;
    if( ! message_done )
    {
        nparsed = http_parser_execute(m_http_parser_ptr, m_http_parser_settings_ptr, buffer, someLength);
    }
    TROG_DEBUG("back from parser nparsed: ", nparsed);
}
enum http_errno Parser::getErrno()
{
    return (enum http_errno) this->m_http_parser_ptr->http_errno;
}
ParserError Parser::getError()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    enum http_errno x = (enum http_errno)this->m_http_parser_ptr->http_errno;
    char* n = (char*)http_errno_name(x);
    char* d = (char*)http_errno_description(x);
    ParserError erst;
    erst.err_number = x;
    erst.name = std::string(n);
    erst.description = std::string(d);
#pragma clang diagnostic pops
    TROG_DEBUG(" errno: ", this->m_http_parser_ptr->http_errno, " name: ", n, " description: ", d);
    return erst;

}
bool Parser::isError(){
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
    enum http_errno x = (enum http_errno)this->m_http_parser_ptr->http_errno;
    char* n = (char*)http_errno_name(x);
    char* d = (char*)http_errno_description(x);
#pragma clang diagnostic pops
    TROG_DEBUG(" errno: ", this->m_http_parser_ptr->http_errno, " name: ", n, " description: ", d);
    // FTROG_DEBUG(" errno: %d name: %s, description: %s", this->parser->http_errno, n,d);
    return (this->m_http_parser_ptr->http_errno != 0) && (this->m_http_parser_ptr->http_errno != HPE_PAUSED);
};

void Parser::p_save_name_value_pair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value)
{
    Parser* p = this;//(Parser*)(parser->data);
    char* n_p = NULL;
    char* v_p = NULL;
    int n;
    n = sb_to_string(name, &n_p);
    n = sb_to_string(value,&v_p);
    
    // Marvin::Headers::canonicalKey(name->buffer, name->used);
    
    std::string n_str = std::string(name->buffer, name->used);
    std::string v_str = std::string(value->buffer, value->used);
    free(v_p);
    free(n_p);
    
    MessageBase* m = p->currentMessage();
    m->setHeader(n_str, v_str);
}
int Parser::p_on_message_begin(http_parser* parser)
{
    return 0;
}
int Parser::p_on_url_data(http_parser* parser, const char* at, size_t length)
{
    Parser* p = this; //(Parser*)(parser->data);
    MessageInterface* message = p->currentMessage();

    if( p->url_buf == NULL)
        p->url_buf = sb_create();
        
    message->setIsRequest(true);
    sb_append( p->url_buf, (char*)at, length);
    return 0;
}
int Parser::p_on_status_data(http_parser* parser, const char* at, size_t length)
{
    Parser* p = this;//(Parser*)(parser->data);
    MessageInterface* message = p->currentMessage();
    if( p->status_buf == NULL)
        p->status_buf = sb_create();
    
    message->setIsRequest(false);
    message->setStatusCode(p->m_http_parser_ptr->status_code);
    sb_append( p->status_buf, (char*)at, length);
    return 0;
}
int Parser::p_on_header_field_data(http_parser* parser, const char* at, size_t length)
{
    Parser* p = this;//(Parser*)(parser->data);
    int state = p->header_state;
    if( (state == 0)||(state == kHEADER_STATE_NOTHING) || (state == kHEADER_STATE_VALUE)){
        if( p->name_buf!= NULL){
            p_save_name_value_pair(parser, p->name_buf, p->value_buf);
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
    } else {
        assert(false);
    }
    p->header_state = kHEADER_STATE_FIELD;
    return 0;
}
int Parser::p_on_header_value_data(http_parser* parser, const char* at, size_t length)
{
    Parser* p = this;//(Parser*)(parser->data);
    int state = p->header_state;
    
    if( state == kHEADER_STATE_FIELD ){
        simple_buffer_t* sb = sb_create();
        sb_append(sb, (char*)at, length);
        p->value_buf = sb;
    } else if( state == kHEADER_STATE_VALUE){
        simple_buffer_t* sb = p->value_buf;
        sb_append(sb, (char*)at, length);
    } else {
        assert(false);
    }
    p->header_state = kHEADER_STATE_VALUE;
    return 0;
}
int Parser::p_on_headers_complete(http_parser* parser) //, const char* aptr, size_t remainder)
{
    Parser* p = this;//(Parser*)(parser->data);
    MessageInterface* message = p->currentMessage();
    
    if( p->name_buf != NULL){
        p_save_name_value_pair(parser, p->name_buf, p->value_buf);
    }
    message->setHttpVersMajor( parser->http_major );
    message->setHttpVersMinor( parser->http_minor );
    if( p->url_buf == NULL ){
    } else {
        message->setMethod((enum http_method)parser->method);
        message->setUri( std::string(p->url_buf->buffer, p->url_buf->used) );
    }
    if( p->status_buf == NULL ){
    } else {
        message->setStatus( std::string(p->status_buf->buffer, p->status_buf->used) );
    }
    p->header_done = true;
    return 0;
}
int Parser::p_on_body_data(http_parser* parser, const char* at, size_t length)
{
    Parser* p = this;//(Parser*)(parser->data);
    BufferChainSPtr chain_sptr = this->m_current_message_ptr->getContentBuffer();
    if (chain_sptr == nullptr) {
        MBufferSPtr mb_sptr = MBuffer::makeSPtr(10000);
        chain_sptr = BufferChain::makeSPtr(mb_sptr);
        m_current_message_ptr->setContentBuffer(chain_sptr);
    }
    chain_sptr->append((void*)at, length);
    return 0;
}
int Parser::p_on_chunk_header(http_parser* parser)
{
    return 0;
}
int Parser::p_on_chunk_complete(http_parser* parser)
{
    return 0;
}
int Parser::p_on_message_complete(http_parser* parser)
{
    Parser* p = this;//(Parser*)(parser->data);
    this->message_done = true;
    
    // MessageInterface* message = p->currentMessage();
    // p->OnMessageComplete(message);
    // force the parser to exit after this call
    // so that we dont process any data in the read
    // buffer beyond the end of the current message
    // in our application that is not possible but lets be careful
    // why not possible - only support one request on a connection
    // at a time - but multiple consecutive requests on the
    // same connection are possible. But a second request
    // wont be accepted until the first one is complete
    http_parser_pause(parser, 1); // TODO fix me
    /*
     * Now get ready for the next message
     */
    return 0;
}

/*
 **************************************************************************************************
 */

#pragma mark - private methods

void Parser::p_setup_next_message()
{
    message_done = false;
    header_done = false;
    
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

void Parser::p_setup_callbacks()
{
    m_http_parser_ptr = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init( m_http_parser_ptr, HTTP_BOTH );
    /** a link back from the C parser to this class*/
    m_http_parser_ptr->data = (void*) this;

    http_parser_settings* settings = (http_parser_settings*)malloc(sizeof(http_parser_settings));
    m_http_parser_settings_ptr = settings;
    
    /* Now set up the call back functions that hook into http_parser*/
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
Parser* xgetParser(http_parser* parser)
{
    return (Parser*) parser->data;
}

int message_begin_cb(http_parser* parser)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_message_begin(parser);;
}
int url_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_url_data(parser, at, length);
}
int status_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_status_data(parser, at, length);
}
int header_field_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_header_field_data(parser, at, length);
}
int header_value_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_header_value_data(parser, at, length);
}
int headers_complete_cb(http_parser* parser) //, const char* aptr, size_t remainder)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_headers_complete(parser);
}
int body_data_cb(http_parser* parser, const char* at, size_t length)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_body_data(parser, at, length);
}
int chunk_header_cb(http_parser* parser)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_chunk_header(parser);
}
int chunk_complete_cb(http_parser* parser)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_chunk_complete(parser);
}
int message_complete_cb(http_parser* parser)
{
    Parser* p =  (Parser*)(parser->data);
    return p->p_on_message_complete(parser);
    // force the parser to exit after this call
    // so that we dont process any data in the read
    // buffer beyond the end of the current message
    // in our application that is not possible but lets be careful
    // why not possible - only support one request on a connection
    // at a time - but multiple consecutive requests on the
    // same connection are possible. But a second request
    // wont be accepted until the first one is complete
    // http_parser_pause(parser, 1); // TODO fix me
    /*
     * Now get ready for the next message
     */
    return 0;
}
} //namespace Marvin
