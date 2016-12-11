#include <string>
#include <map>
#include <sstream>
#include "http_parser.h"
#include "message.hpp"


#pragma - http message base impl

void
MessageBase::setStatusCode(int sc){ _status_code = sc;}

void
MessageBase::setStatus(std::string st){ _status = st;}

int
MessageBase::statusCode() {return _status_code; };

std::string
MessageBase::status() {return _status;};

void
MessageBase::setMethod(enum http_method m){ _method = m;}

std::string
MessageBase::getMethodAsString(){return std::string("");};

void
MessageBase::setUrl(std::string u){ _url = u;}

std::string
MessageBase::url(){ return _url; }

void
MessageBase::setHttpVersMajor(int major){ _http_major = major; }

int
MessageBase::httpVersMajor() {return _http_major; }

void
MessageBase::setHttpVersMinor(int minor){ _http_minor = minor; }

int
MessageBase::httpVersMinor(){return _http_minor; }

void
MessageBase::setHeader(std::string key, std::string value){ _headers[key] = value; };

bool
MessageBase::hasHeader( std::string key){ return ( _headers.find(key) != _headers.end() ); };

std::string
MessageBase::header(std::string key){ if( hasHeader(key) ){ return _headers[key]; } else { return nullptr;}  }

void
MessageBase::removeHeader( std::string key){
    if( _headers.find(key) != _headers.end()  )
        _headers.erase(key);
}

std::string
MessageBase::getHeader(std::string key){
    if( _headers.find(key) != _headers.end() ){
        return _headers[key];
    }
    return nullptr;
}
std::map<std::string, std::string>
MessageBase::getHeaders(){
    return _headers;
}
std::string
MessageBase::str(){
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
MessageBase::dumpHeaders(std::ostream& os)
{
    std::map<std::string, std::string>::iterator it = _headers.begin();
    while(it != _headers.end())
    {
        os<<it->first<<" : "<<it->second<<std::endl;
        it++;
        }
        
}
void
MessageBase::setTrailer(std::string key, std::string value){ _trailers[key] = value; };
bool
MessageBase::hasTrailer( std::string key){ return ( _trailers.find(key) != _trailers.end() ); };
std::string
MessageBase::trailer(std::string key){if( hasTrailer(key) ){return _trailers[key];} else{ return nullptr;} }
    
