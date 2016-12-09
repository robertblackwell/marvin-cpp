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
    void setStatusCode(int sc){ _status_code = sc;}
    void setStatus(std::string st){ _status = st;}
    int  statusCode() {return _status_code; };
    std::string status() {return _status;};

    void setMethod(enum http_method m){ _method = m;}
    std::string getMethodAsString(){return std::string("");};
    
    void setUrl(std::string u){ _url = u;}
    std::string url(){ return _url; }
    
    void setHttpVersMajor(int major){ _http_major = major; }
    int  httpVersMajor() {return _http_major; }
    
    void setHttpVersMinor(int minor){ _http_minor = minor; }
    int  httpVersMinor(){return _http_minor; }
    
    void setHeader(std::string key, std::string value){ _headers[key] = value; };
    bool hasHeader( std::string key){ return ( _headers.find(key) != _headers.end() ); };
    std::string header(std::string key){ if( hasHeader(key) ){ return _headers[key]; } else { return nullptr;}  }
    
    void
    removeHeader( std::string key){
        if( _headers.find(key) != _headers.end()  )
            _headers.erase(key);
    }
    
    std::string
    getHeader(std::string key){
        if( _headers.find(key) != _headers.end() ){
            return _headers[key];
        }
        return nullptr;
    }
    std::map<std::string, std::string> getHeaders(){
        return _headers;
    }
    std::string str(){
        std::ostringstream ss;
        ss << "HTTP/" << httpVersMajor() << "." << httpVersMinor() << " " << statusCode() << " " << status() << "\r\n";
        std::map<std::string, std::string>::iterator it = _headers.begin();
        while(it != _headers.end())
        {
            ss << it->first << ": " << it->second << "\r\n";
            it++;
        }
        ss << "\r\n";
        return ss.str();
    }
    void
    dumpHeaders(std::ostream& os)
    {
        std::map<std::string, std::string>::iterator it = _headers.begin();
        while(it != _headers.end())
        {
            os<<it->first<<" : "<<it->second<<std::endl;
            it++;
            }
            
    }
    void
    setTrailer(std::string key, std::string value){ _trailers[key] = value; };
    bool hasTrailer( std::string key){ return ( _trailers.find(key) != _trailers.end() ); };
    std::string trailer(std::string key){if( hasTrailer(key) ){return _trailers[key];} else{ return nullptr;} }
    
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
