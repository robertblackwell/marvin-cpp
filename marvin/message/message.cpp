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

//void serializeHeaders(MessageBase& msg, boost::asio::streambuf& b)
void serializeHeaders(MessageBase& msg, Marvin::MBuffer& mb)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    std::string vers = "HTTP/" + std::to_string(msg.httpVersMajor()) + "." + std::to_string(msg.httpVersMinor());
    if( msg.isRequest() ){
        std::string m = msg.getMethodAsString();
        std::string u = msg.uri();
        os << m << " " << u << " " << vers << "\r\n";
    } else{
        os << vers << " " << msg.m_status_code << " " << msg.m_status <<  "\r\n";
    }
    
    for(auto const& h : msg.m_headers) {
        os << h.first << ": " << h.second << "\r\n";
    }
    // end of headers
    os << "\r\n";
    // debugging only
    boost::asio::streambuf::const_buffers_type bufs = b.data();

    std::string ss(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + b.size() );
    mb.append((void*) ss.c_str(), ss.size());
//    return b;
    std::string sss = ss;
    
}
#pragma - http message base impl

MessageBase::MessageBase()
{
    this->m_is_request = true;
    this->setHttpVersMajor(1);
    this->setHttpVersMinor(1);
}

MessageBase::~MessageBase(){}

bool
MessageBase::isRequest(){ return m_is_request; }

void
MessageBase::setIsRequest(bool flag){ m_is_request = flag;}

void
MessageBase::setStatusCode(int sc){ m_status_code = sc; m_is_request = false; }

void
MessageBase::setStatus(std::string st){ m_status = st; m_is_request = false; }

int
MessageBase::statusCode() {return m_status_code; };

std::string
MessageBase::status() {return m_status;};

void
MessageBase::setMethod(HttpMethod m){ m_method = (enum http_method)m;  m_is_request = true; }
void
MessageBase::setMethod(enum http_method m){ m_method = m; m_is_request = true; }
void
MessageBase::setMethod(std::string m){ m_methodStr = m; m_is_request = true; }

std::string
MessageBase::getMethodAsString(){return httpMethodString((HttpMethod) m_method);};

void
MessageBase::setUri(std::string u){ m_uri = u;}

std::string
MessageBase::uri(){
return m_uri;
}

void
MessageBase::setHttpVersMajor(int major){ m_http_major = major; }

int
MessageBase::httpVersMajor() {return m_http_major; }

void
MessageBase::setHttpVersMinor(int minor){ m_http_minor = minor; }

int
MessageBase::httpVersMinor(){return m_http_minor; }

void
MessageBase::setHeader(std::string key, std::string value){
    HttpHeader::canonicalKey(key);
    m_headers[key] = value;
};

bool
MessageBase::hasHeader( std::string key){
    HttpHeader::canonicalKey(key);
    return ( m_headers.find(key) != m_headers.end() );
};

std::string
MessageBase::header(std::string key){
    HttpHeader::canonicalKey(key);
    if( hasHeader(key) ){ return m_headers[key]; } else { return nullptr;}
}

void
MessageBase::removeHeader( std::string key){
    HttpHeader::canonicalKey(key);
    if( m_headers.find(key) != m_headers.end()  )
        m_headers.erase(key);
}

std::string
MessageBase::getHeader(std::string key){
    HttpHeader::canonicalKey(key);
    if( m_headers.find(key) != m_headers.end() ){
        return m_headers[key];
    }
    return nullptr;
}
//std::map<std::string, std::string>&
HttpHeadersType&
MessageBase::getHeaders(){
    return m_headers;
}
std::string
MessageBase::str(){
    std::ostringstream ss;
    ss << "HTTP/" << httpVersMajor() << "." << httpVersMinor() << " " << statusCode() << " " << status() << "\r\n";
    std::map<std::string, std::string>::iterator it = m_headers.begin();
    while(it != m_headers.end())
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
    std::map<std::string, std::string>::iterator it = m_headers.begin();
    while(it != m_headers.end())
    {
        os<<it->first<<" : "<<it->second<<std::endl;
        it++;
        }
        
}
void
MessageBase::setTrailer(std::string key, std::string value){ m_trailers[key] = value; };
bool
MessageBase::hasTrailer( std::string key){ return ( m_trailers.find(key) != m_trailers.end() ); };
std::string
MessageBase::trailer(std::string key){if( hasTrailer(key) ){return m_trailers[key];} else{ return nullptr;} }
    
