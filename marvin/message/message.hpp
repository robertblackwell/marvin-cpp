#ifndef marvin_http_message_hpp
#define marvin_http_message_hpp
#include <string>
#include <map>
#include <sstream>
#include "buffer.hpp"
#include "http_parser.h"
#include "boost_stuff.hpp"
#include "i_message.hpp"
/**
* \defgroup HttpMessage
* \brief This group deals with the structure, parsing and construction of HTTP/1.1 messages.
*/
#pragma once
#pragma mark - http message interfaces

/**
* \ingroup HttpMessage
* \brief Enum for HTTP method. It is a BAD kludge to get C++ enum same as http_parser c enum;
* usage HttpMethod::GET -- see http_parser.h for list of names
*/
enum class HttpMethod{
#define EC(num, name, string) name = HTTP_##name,
    HTTP_METHOD_MAP(EC)
#undef EC
};
std::string httpMethodString(HttpMethod m);
/**
* \ingroup HttpMessage
* \brief Defines an interface that all representations of a Http Message should conform to.
*/
using MessageInterface = IMessage;
#if 0
class MessageInterface
{
public:
    virtual void setMethod(HttpMethod m) = 0 ;
    virtual void setMethod(enum http_method m) = 0 ;
    virtual void setMethod(std::string m_str) = 0;
//    virtual std::string method() = 0;
    
    virtual void setUri(std::string u) = 0 ;
    virtual std::string uri() = 0;
    
    virtual void setStatusCode(int sc) = 0 ;
    virtual int  statusCode() = 0;
    
    virtual void setStatus(std::string st) = 0 ;
    virtual std::string status() = 0;
    
    virtual void setHttpVersMajor(int major) = 0 ;
    virtual int  httpVersMajor() = 0 ;

    virtual void setHttpVersMinor(int minor) = 0 ;
    virtual int  httpVersMinor() = 0 ;

    virtual void setHeader(std::string key, std::string value) = 0 ;
    virtual std::string header(std::string key) = 0;
    
    virtual void setTrailer(std::string key, std::string value) = 0;
    virtual std::string trailer(std::string key) = 0;
    
    virtual void setIsRequest(bool isrreq) = 0;
    virtual bool isRequest() = 0;
    
};
#endif
#pragma - http message base
class MessageBase;
/// \ingroup HttpMessage
typedef std::shared_ptr<MessageBase> MessageBaseSPtr;

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
    HttpMethod method(){ return (HttpMethod)_method; };
    
    void setUri(std::string u);
    std::string uri();
    
    void setHttpVersMajor(int major);
    int  httpVersMajor();
    
    void setHttpVersMinor(int minor);
    int  httpVersMinor();
    
    void setHeader(std::string key, std::string value);
    bool hasHeader( std::string key);
    std::string header(std::string key);
    
    void removeHeader( std::string key);
    
    std::string getHeader(std::string key);

    std::map<std::string, std::string>& getHeaders();
    
    std::string  str();
    void    dumpHeaders(std::ostream& os);
    void    setTrailer(std::string key, std::string value);
    bool    hasTrailer( std::string key);
    std::string trailer(std::string key);

    void    setIsRequest(bool flag);
    bool    isRequest();
    
    friend std::string traceMessage(MessageBase& msg);
    friend void serializeHeaders(MessageBase& msg, Marvin::MBuffer& buf);
//    friend void serializeHeaders(MessageBase& msg, boost::asio::streambuf& buf);

protected:

    bool                                _is_request;
    enum http_method  					_method;
    std::string                         _methodStr;
    std::string							_uri;

    int									_status_code;
    std::string							_status;

    int									_http_major;
    int									_http_minor;
    std::map<std::string, std::string>	_headers;
    std::map<std::string, std::string>	_trailers;
    
};

#endif
