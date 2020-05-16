#ifndef marvin_http_message_hpp
#define marvin_http_message_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <http-parser/http_parser.h>
#include <marvin/http/http_method.hpp>
#include <marvin/http/headers_v2.hpp>
namespace Marvin {

class MessageBase;

using MessageBaseSPtr = std::shared_ptr<MessageBase>;
std::string traceMessage(MessageBase& msg);

void serialize_headers(MessageBase& msg, std::string& string);
void serialize_headers(MessageBase& msg, ContigBufferSPtr mb);
ContigBufferSPtr serialize_headers(MessageBase& msg);
bool isConnectionKeepAlive(Marvin::MessageBase& msg);

/**
* \brief A class that can represent a http message either standalone or as a mixin for other classes; See MessageReader for an example.
*/class MessageBase
{
public:
    MessageBase();
    MessageBase(MessageBase& other);
    MessageBase(MessageBase&& other);
    MessageBase& operator =(MessageBase& other);
    MessageBase& operator =(MessageBase&& other);
    
    ~MessageBase();

    /// new interface
    bool is_request();

    void status_code(int code);
    int status_code();

    std::string reason();
    void reason(std::string reason);

    HttpMethod method();
    void method(HttpMethod);
    void method(enum http_method m);
    std::string method_string();

    void target(std::string target);
    std::string target();

    void version(int major, int minor);
    void version_major(int major);
    void version_minor(int minor);
    int  version_minor();
    int  version_major();

    HeadersV2& headers();
    boost::optional<std::string> header(std::string key);
    void header(std::string key, std::string value);
    void header(std::string* k, std::string* v);
    void remove_header( std::string key);
    boost::optional<std::size_t> contentLength();
    
    HeadersV2& trailers();
    boost::optional<std::string> trailer(std::string key);
    void trailer(std::string key, std::string value);

    void dumpHeaders(std::ostream& os);

    /** serialize the message prefix = firstline + headers lines*/
    void serialize_headers(ContigBufferSPtr mb);
    /** serialize the message prefix = firstline + headers lines*/
    ContigBufferSPtr serialize_headers();
    
    // std::string  str();

    void    setIsRequest(bool flag);
    bool    isRequest();
    
    /// \brief return the current dechunked content buffer
    ::Marvin::BufferChainSPtr getContentBuffer();
    /// Sets the dechunked content buffer but does not set the CONTENT-LENGTH headers;
    /// this is because even for chunked incoming messages the de-chunked buffer is attached
    void setContentBuffer(::Marvin::BufferChainSPtr bufSPtr);

    /// Returns the current dechunked conent buffer
    ::Marvin::BufferChainSPtr getContent();
    /// Sets the dechunked content buffer and also updates the content-length field;
    /// use this method when preparing an outgoing message
    void setContent(::Marvin::BufferChainSPtr bufSPtr);
    /// Sets the dechunked content buffer and aslo updates the content-length field;
    /// use this method when preparing an outgoing message
    void setContent(std::string content);

    std::string to_string();
#if 1
    // friend void serialize_headers(MessageBase& msg, ::Marvin::ContigBuffer& buf);
    // friend Marvin::ContigBufferSPtr serialize_headers(MessageBase& msg);
    // friend std::string serialize_headers(MessageBase& msg);
    friend void serialize_headers(MessageBase& msg, std::string& str);
    // friend void serialize_headers(MessageBase& msg, BufferChainSPtr bchain_sptr);
    friend void serialize_headers(MessageBase& msg, ContigBufferSPtr mb);
    friend ContigBufferSPtr serialize_headers(MessageBase& msg);
#endif
    friend std::string traceMessage(MessageBase& msg);
    friend std::ostream &operator<< (std::ostream &os, MessageBase &msg);

protected:

    bool                        m_is_request;
    // store method enum as in http-parser
    enum http_method  			m_method;
    std::string                 m_method_str;
    std::string					m_target;
    int							m_status_code;
    std::string					m_reason;
    int							m_http_major;
    int							m_http_minor;
    Marvin::HeadersV2           m_headers;
    Marvin::HeadersV2           m_trailers;
    Marvin::BufferChainSPtr     m_body_chain_sptr;
    
};

} // namespace Marvin
#endif
