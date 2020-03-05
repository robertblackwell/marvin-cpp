#ifndef marvin_http_i_message_hpp
#define marvin_http_i_message_hpp
#include <string>
#include <map>
#include <sstream>
#include <marvin/buffer/buffer.hpp>
#include <marvin/external_src/http-parser/http_parser.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/http/http_method.hpp>
#pragma mark - http message interfaces

namespace Marvin {
namespace Http {
/**
* \ingroup HttpMessage
* \brief Defines an interface that all representations of a Http Message should conform to.
*/
class IMessage
{
public:
    virtual void setMethod(HttpMethod m) = 0 ;
    virtual void setMethod(enum http_method m) = 0 ;
    virtual void setMethod(std::string m_str) = 0;
//    virtual std::string method() = 0;
    virtual std::string getMethodAsString() = 0;
    virtual HttpMethod method() = 0;

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
} //namespace Http
} //namespace Marvin
#endif
