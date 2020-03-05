#ifndef marvin_http_message_hpp
#define marvin_http_message_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/boost_stuff.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/external_src/http-parser/http_parser.h>
#include <marvin/http/http_header.hpp>
#include <marvin/http/message_interface.hpp>
namespace Marvin {
namespace Http {
/**
* \defgroup HttpMessage
* \brief This group deals with the structure, parsing and construction of HTTP/1.1 messages.
*/
#pragma once
#pragma mark - http message interfaces
/**
* \ingroup HttpMessage
* \brief Defines an interface that all representations of a Http Message should conform to.
*/
using MessageInterface = IMessage;

#pragma - http message base
class MessageBase;
/// \ingroup HttpMessage
using MessageBaseSPtr = std::shared_ptr<MessageBase>;
std::string traceMessage(MessageBase& msg);
void serializeHeaders(MessageBase& msg, ::Marvin::MBuffer& buf);
Marvin::MBufferSPtr serializeHeaders(MessageBase& msg);

/// \ingroup HttpMessage
/// \brief A class that can represent a http message either standalone or as a mixin for other classes; See MessageReader for an example.
class MessageBase : public MessageInterface
{
public:
    MessageBase();
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
    
    void setHttpVersMajor(int major);
    int  httpVersMajor();
    
    void setHttpVersMinor(int minor);
    int  httpVersMinor();
    
    Marvin::Http::Headers& getHeaders();
    bool hasHeader( std::string key);
    std::string header(std::string key);
    std::string getHeader(std::string key);
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
    /// \vrief sets the dechunked content buffer but does not set the CONTENT-LENGTH headers;
    /// this is because even for chunked incoming messages the de-chunked buffer is attached
    void setContentBuffer(::Marvin::BufferChainSPtr bufSPtr);

    /// \brief return the current dechunked conent buffer
    ::Marvin::BufferChainSPtr getContent();
    /// \brief sets the dechunked content buffer and aslo updates the content-length field;
    /// use this method when preparing an outgoing message
    void setContent(::Marvin::BufferChainSPtr bufSPtr);
    /// \brief sets the dechunked content buffer and aslo updates the content-length field;
    /// use this method when preparing an outgoing message
    void setContent(std::string content);

    friend std::string traceMessage(MessageBase& msg);
    friend void serializeHeaders(MessageBase& msg, ::Marvin::MBuffer& buf);
    friend Marvin::MBufferSPtr serializeHeaders(MessageBase& msg);
//    friend void serializeHeaders(MessageBase& msg, boost::asio::streambuf& buf);
    friend std::ostream &operator<< (std::ostream &os, MessageBase &msg);

protected:

    bool                                m_is_request;
    enum http_method  					m_method;
    std::string                         m_methodStr;
    std::string							m_uri;

    int									m_status_code;
    std::string							m_status;

    int									m_http_major;
    int									m_http_minor;

//    Marvin::Http::Headers                     m_headers;
//    Marvin::Http::Headers                     m_trailers;
    Marvin::Http::Headers               m_headers;
    Marvin::Http::Headers               m_trailers;

    Marvin::BufferChainSPtr             m_body_chain_sptr;
    
};

} // namespace Http
} // namespace Marvin
#endif
