#include <string>
#include <map>
#include <sstream>
#include "http_parser.h"
#include "http_header.hpp"
#include "message.hpp"

std::string traceMessage(MessageBase& msg)
{
    std::stringstream ss;
    ss << std::hex << &msg << std::dec << " ";
    
    if( msg.isRequest() ){
        ss << msg.getMethodAsString() << " " << msg.uri() << " HTTP/1." << msg.httpVersMinor();
    }else{
        ss << "HTTP/1." << msg.httpVersMinor() << " " << msg.statusCode() << " " << msg.status();
    }
    if( msg.hasHeader(HttpHeader::Name::ContentLength) ){
        ss << "ContentLen : " << msg.getHeader(HttpHeader::Name::ContentLength);
    }else{
        ss << "No content length header";
    }
    if( msg.hasHeader(HttpHeader::Name::TransferEncoding) ){
        ss << "TransferEncodin : " << msg.getHeader(HttpHeader::Name::TransferEncoding);
    }else{
        ss << "No transfer encoding";
    }
    std::string s = ss.str();
    return ss.str();
}


std::string httpMethodString(HttpMethod m){

    enum http_method c_m = (enum http_method) m;
    std::string s(http_method_str(c_m));
    return s;
}

#pragma - http message base impl

bool
MessageBase::isRequest(){ return _isRequest; }

void
MessageBase::setIsRequest(bool flag){ _isRequest = flag;}

void
MessageBase::setStatusCode(int sc){ _status_code = sc;}

void
MessageBase::setStatus(std::string st){ _status = st;}

int
MessageBase::statusCode() {return _status_code; };

std::string
MessageBase::status() {return _status;};

void
MessageBase::setMethod(HttpMethod m){ _method = (enum http_method)m;}
void
MessageBase::setMethod(enum http_method m){ _method = m;}
void
MessageBase::setMethod(std::string m){ _methodStr = m;}

std::string
MessageBase::getMethodAsString(){return httpMethodString((HttpMethod) _method);};

void
MessageBase::setUri(std::string u){ _uri = u;}

std::string
MessageBase::uri(){ return _uri; }

void
MessageBase::setHttpVersMajor(int major){ _http_major = major; }

int
MessageBase::httpVersMajor() {return _http_major; }

void
MessageBase::setHttpVersMinor(int minor){ _http_minor = minor; }

int
MessageBase::httpVersMinor(){return _http_minor; }

void
MessageBase::setHeader(std::string key, std::string value){
    HttpHeader::canonicalKey(key);
    _headers[key] = value;
};

bool
MessageBase::hasHeader( std::string key){
    HttpHeader::canonicalKey(key);
    return ( _headers.find(key) != _headers.end() );
};

std::string
MessageBase::header(std::string key){
    HttpHeader::canonicalKey(key);
    if( hasHeader(key) ){ return _headers[key]; } else { return nullptr;}
}

void
MessageBase::removeHeader( std::string key){
    HttpHeader::canonicalKey(key);
    if( _headers.find(key) != _headers.end()  )
        _headers.erase(key);
}

std::string
MessageBase::getHeader(std::string key){
    HttpHeader::canonicalKey(key);
    if( _headers.find(key) != _headers.end() ){
        return _headers[key];
    }
    return nullptr;
}
std::map<std::string, std::string>&
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
    
