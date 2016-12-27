#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <string>
#include <map>
#include <sstream>
#include "http_parser.h"

#pragma once
#pragma mark - http message interfaces

//
// BAD kludge to get C++ enum same as http_parser c enum
//
//  usage HttpMethod::GET -- see http_parser.h for list of names
//
enum class HttpMethod{
#define EC(num, name, string) name = HTTP_##name,
    HTTP_METHOD_MAP(EC)
#undef EC
};
std::string httpMethodString(HttpMethod m);

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

#pragma - http message base

class MessageBase : public MessageInterface
{
public:
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
    
    void
    removeHeader( std::string key);
    
    std::string
    getHeader(std::string key);

    std::map<std::string, std::string>&
    getHeaders();
    
    std::string
    str();
    void
    dumpHeaders(std::ostream& os);
    void
    setTrailer(std::string key, std::string value);
    bool
    hasTrailer( std::string key);
    std::string trailer(std::string key);

    void    setIsRequest(bool flag);
    bool    isRequest();
    
    friend std::string traceMessage(MessageBase& msg);

protected:

    bool                                _isRequest;
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
