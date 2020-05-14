#ifndef marvin_http_message_hpp
#define marvin_http_message_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include<http-parser/http_parser.h>
#include <marvin/http/headers_v2.hpp>
#include <marvin/http/message_interface.hpp>
namespace Marvin {

using MessageInterface = IMessage;
class MessageBase;

using MessageBaseSPtr = std::shared_ptr<MessageBase>;
std::string traceMessage(MessageBase& msg);
void serializeHeaders(MessageBase& msg, ::Marvin::MBuffer& buf);
Marvin::MBufferSPtr serializeHeaders(MessageBase& msg);
//void serialize_headers(MessageBase& msg, BufferChainSPtr bchain_sptr);
void serialize(MessageBase& msg, MBufferSPtr mb);

/**
* \defgroup http Http Message 
* \brief Classes that collectively implement details of the http message format.
*/
///
/// KeepAlive is true if:
///     there is a connection header that contains the string "[ ,]keep-alive[ ,]" case independent
///     or
///     there is NOT a connection header that contain the string 'close' case insensitive
///         and the msg http version is 1.1
///
/// Keepalive is explicitly false if
///     there is a connection header that contain the string 'close' case insensitive
///     or
///     there is NOT a connection header that contain the string 'keep-alive' case insensitive
///         and the msg http version is 1.0
///
bool isConnectionKeepAlive(Marvin::MessageBase& msg);

/**
* \brief A class that can represent a http message either standalone or as a mixin for other classes; See MessageReader for an example.
*/class MessageBase : public MessageInterface
{
public:
    MessageBase();
    MessageBase(MessageBase& other);
    MessageBase(MessageBase&& other);
    MessageBase& operator =(MessageBase& other);
    MessageBase& operator =(MessageBase&& other);
    
    ~MessageBase();
    void setStatusCode(int sc);
    void setStatus(std::string st);
    int  statusCode();
    std::string status();

    void setMethod(HttpMethod m);
    void setMethod(enum http_method m);
    virtual void setMethod(std::string m_str);
    std::string getMethodAsString();
    HttpMethod method(){ return (HttpMethod)m_method; };
    
    void setUri(std::string u);
    std::string uri();
    std::string getPath();
    void setPath(std::string path);
    
    void setHttpVersMajor(int major);
    int  httpVersMajor();
    
    void setHttpVersMinor(int minor);
    int  httpVersMinor();
    
    Marvin::HeadersV2& getHeaders();
    bool hasHeader(std::string key);
    std::string header(std::string key);
    std::string getHeader(std::string key);
    std::size_t contentLength();
    void setHeader(std::string key, std::string value);
    void removeHeader( std::string key);
    void dumpHeaders(std::ostream& os);
    
    void setTrailer(std::string key, std::string value);
    bool hasTrailer( std::string key);
    std::string trailer(std::string key);

    std::string  str();

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

    friend std::string traceMessage(MessageBase& msg);
    friend void serializeHeaders(MessageBase& msg, ::Marvin::MBuffer& buf);
    friend Marvin::MBufferSPtr serializeHeaders(MessageBase& msg);
    friend std::string serialize_headers(MessageBase& msg);
    friend void serialize_headers(MessageBase& msg, std::string& str);
    friend void serialize_headers(MessageBase& msg, BufferChainSPtr bchain_sptr);
    friend void serialize(MessageBase& msg, MBufferSPtr mb);

    //    friend void serializeHeaders(MessageBase& msg, boost::asio::streambuf& buf);
    friend std::ostream &operator<< (std::ostream &os, MessageBase &msg);

protected:

    bool                        m_is_request;
    enum http_method  			m_method;
    std::string                 m_methodStr;
    std::string					m_uri;
    int							m_status_code;
    std::string					m_status;
    int							m_http_major;
    int							m_http_minor;
    Marvin::HeadersV2           m_headers;
    Marvin::HeadersV2           m_trailers;
    Marvin::BufferChainSPtr     m_body_chain_sptr;
    
};

} // namespace Marvin
#endif
