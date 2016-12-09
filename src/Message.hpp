#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <string>
#include <map>
#include <sstream>
#include "http_parser.h"

#pragma once
#pragma mark - http message interfaces

class MessageInterface
{
public:
    virtual void setMethod(enum http_method m) = 0 ;
//    virtual std::string method() = 0;
    
    virtual void setUrl(std::string u) = 0 ;
    virtual std::string url() = 0;
    
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
};

#pragma - http message base

class MessageBase : public MessageInterface
{
public:
    void setStatusCode(int sc);
    void setStatus(std::string st);
    int  statusCode();
    std::string status();

    void setMethod(enum http_method m);
    std::string getMethodAsString();
    
    void setUrl(std::string u);
    std::string url();
    
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

    std::map<std::string, std::string>
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
    
private:
    enum http_method  					_method;
    std::string							_url;

    int									_status_code;
    std::string							_status;

    int									_http_major;
    int									_http_minor;
    std::map<std::string, std::string>	_headers;
    std::map<std::string, std::string>	_trailers;
    
};

#endif
