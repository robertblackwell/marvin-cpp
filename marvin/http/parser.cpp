#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>

#include <marvin/http/parser.hpp>

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <marvin/http/header_fields.hpp>
#include <marvin/error_handler/error_handler.hpp>


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

Parser::Parser(): m_buffer_strategy(BodyAllocator()), m_factory(m_buffer_strategy)
{
    message_done = false;
    header_done = false;
    m_http_parser_ptr = NULL;
    m_http_parser_settings_ptr = NULL;
    header_state = kHEADER_STATE_NOTHING;
}

Parser::~Parser()
{
    if (m_http_parser_ptr != NULL) {free(m_http_parser_ptr);m_http_parser_ptr = NULL;}
    if (m_http_parser_ptr != NULL) {free(m_http_parser_settings_ptr); m_http_parser_settings_ptr = NULL;}
}
MessageBase::SPtr Parser::current_message()
{
    return m_current_message_sptr;
}
int Parser::appendBytes(void *buffer, unsigned length)
{
    std::string tmp = std::string((char*)buffer, length);
    size_t nparsed = http_parser_execute(m_http_parser_ptr, m_http_parser_settings_ptr, (char*)buffer, (int)length);
    return (int)nparsed;
}
void Parser::begin(MessageBase::SPtr message_sptr)
{
    p_initialize();
    this->m_current_message_sptr = message_sptr;
}
Parser::ReturnValue Parser::consume(boost::asio::streambuf& streambuffer, bool only_header)
{
    started = true;
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
    started = true;
    ReturnValue rv{.return_code = ReturnCode::end_of_data, .bytes_remaining = length};
    char* b = (char*) buf;
    std::size_t total_parsed = 0;
    while (total_parsed < length) {
        char* b_start_ptr = &(b[total_parsed]);
        int nparsed = this->appendBytes((void*) b_start_ptr, length - total_parsed);
        total_parsed = total_parsed + nparsed;
        // std::cout << "nparsed: " << nparsed  << "len: " << length - total_parsed << " content: " << buf <<  std::endl;
        rv.bytes_remaining = length - nparsed;
        if (this->is_error()) {
            rv.return_code = ReturnCode::error;
            auto x = get_error();
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
        if (started) {
            nparsed = http_parser_execute(m_http_parser_ptr, m_http_parser_settings_ptr, buffer, someLength);
        }
        if (this->is_error()) {
            rv.return_code = ReturnCode::error;
            auto x = get_error();
            return rv;
        } else if (this->message_done) {
            rv.return_code = ReturnCode::end_of_message;
            return rv;
        } else if (this->header_done) {
            rv.return_code = ReturnCode::end_of_header;
        } else {
            std::cout << "should not be here" << std::endl;
            // MARVIN_THROW("dont think we should get here");
            return rv;
        }
        return rv;
    }
    return rv;
}
void Parser::append_eof()
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
enum http_errno Parser::get_errno()
{
    return (enum http_errno) this->m_http_parser_ptr->http_errno;
}
ParserError Parser::get_error()
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
bool Parser::is_error(){
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
int Parser::p_on_message_begin(http_parser* parser)
{
    status_stringbuf.clear();
    url_stringbuf.clear();
    name_stringbuf.clear();
    value_stringbuf.clear();
    return 0;
}
int Parser::p_on_url_data(http_parser* parser, const char* at, size_t length)
{
    MessageBase::SPtr message = current_message();
    message->set_is_request(true);
    url_stringbuf.append((char*)at, length);
    return 0;
}
int Parser::p_on_status_data(http_parser* parser, const char* at, size_t length)
{
    MessageBase::SPtr message = current_message();
    message->set_is_request(false);
    message->status_code(m_http_parser_ptr->status_code);
    status_stringbuf.append((char*)at, length);
    return 0;
}
int Parser::p_on_header_field_data(http_parser* parser, const char* at, size_t length)
{
    int state = header_state;
    if( (state == 0)||(state == kHEADER_STATE_NOTHING) || (state == kHEADER_STATE_VALUE)){
        if(name_stringbuf.size() != 0) {
            current_message()->header(&name_stringbuf, &value_stringbuf);
            name_stringbuf.clear();
            value_stringbuf.clear();
        }
        name_stringbuf.append((char*)at, length);
    } else if( state == kHEADER_STATE_FIELD){
        name_stringbuf.append((char*)at, length);
    } else {
        assert(false);
    }
    header_state = kHEADER_STATE_FIELD;
    return 0;
}
int Parser::p_on_header_value_data(http_parser* parser, const char* at, size_t length)
{
    int state = header_state;
    if( state == kHEADER_STATE_FIELD ){
        value_stringbuf.clear();
        value_stringbuf.reserve(HEADER_RESERVE_SIZE);
        value_stringbuf.append((char*)at, length);
    } else if( state == kHEADER_STATE_VALUE){
        value_stringbuf.append((char*)at, length);
    } else {
        assert(false);
    }
    header_state = kHEADER_STATE_VALUE;
    return 0;
}
int Parser::p_on_headers_complete(http_parser* parser) //, const char* aptr, size_t remainder)
{
    MessageBase::SPtr message = current_message();
    if(name_stringbuf.size() != 0) {
        message->header(name_stringbuf, value_stringbuf);
        name_stringbuf.clear();
        value_stringbuf.clear();
    }
    message->version( parser->http_major, parser->http_minor );
    if( url_stringbuf.size()  == 0 ){
    } else {
        message->method((enum http_method)parser->method);
        message->target(url_stringbuf);
    }
    if( status_stringbuf.size() == 0 ){
    } else {
        message->reason(status_stringbuf);
    }
    header_done = true;
    return 0;
}
int Parser::p_on_body_data(http_parser* parser, const char* at, size_t length)
{
    BufferChain::SPtr chain_sptr = this->m_current_message_sptr->get_body_buffer_chain();
    if (chain_sptr == nullptr) {
        ContigBuffer::SPtr mb_sptr = m_factory.makeSPtr();
        chain_sptr = makeBufferChainSPtr(mb_sptr);
        m_current_message_sptr->set_body_buffer_chain(chain_sptr);
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
    
    // MessageBase* message = p->current_message();
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
void Parser::p_initialize()
{
    header_state = kHEADER_STATE_NOTHING;
    started = false;
    message_done = false;
    header_done = false;
    m_current_message_sptr = nullptr;
    if (m_http_parser_ptr != NULL) {
        free(m_http_parser_ptr);
    }
    m_http_parser_ptr = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init( m_http_parser_ptr, HTTP_BOTH );
    /** a link back from the C parser to this class*/
    m_http_parser_ptr->data = (void*) this;

    if (m_http_parser_settings_ptr != NULL) {
        free(m_http_parser_settings_ptr);
    }
    http_parser_settings* settings = (http_parser_settings*)malloc(sizeof(http_parser_settings));
    m_http_parser_settings_ptr = settings;
    status_stringbuf.clear();
    url_stringbuf.clear();
    name_stringbuf.clear();
    value_stringbuf.clear();
    m_http_parser_settings_ptr->on_message_begin = message_begin_cb;
    m_http_parser_settings_ptr->on_url = url_data_cb;
    m_http_parser_settings_ptr->on_status = status_data_cb;
    m_http_parser_settings_ptr->on_header_field = header_field_data_cb;
    m_http_parser_settings_ptr->on_header_value = header_value_data_cb;
    m_http_parser_settings_ptr->on_headers_complete = headers_complete_cb;
    m_http_parser_settings_ptr->on_body = body_data_cb;
    m_http_parser_settings_ptr->on_message_complete = message_complete_cb;
    m_http_parser_settings_ptr->on_chunk_header = chunk_header_cb;
    m_http_parser_settings_ptr->on_chunk_complete = chunk_complete_cb;
}
#pragma mark - c-language call back functions implementation

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
