//
//  BRNHTTPMessage.m
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/18/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <marvin/HTTPMessage.hpp>
#include <marvin/http/parser.hpp>


#pragma mark - http method stuff
/*********************************************************************************/
static const char *_method_strings[] =
{
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};


std::string methodStringFromEnum(enum http_method method)
{
    std::string x = std::string(_method_strings[method]);
    return x;
}
/*********************************************************************************/
const std::string HTTPMessage::CONTENT_LENGTH = "Content-Length";
const std::string HTTPMessage::CONNECTION = "Connection";



bool HTTPMessage::hasHeader(std::string key)
{
    if( headers.find(key) != headers.end()  )
        return true;
    return false;
}

void HTTPMessage::removeHeader(std::string key)
{
    if( headers.find(key) != headers.end()  )
        headers.erase(key);
}
void HTTPMessage::setHeader(std::string key, std::string value)
{
    headers[key] = value;
}
std::string HTTPMessage::getHeader(std::string key)
{
    if( headers.find(key) != headers.end()  )
        return headers[key];
    return nullptr;
}
void HTTPMessage::dumpHeaders(std::ostream& os)
{
    std::map<std::string, std::string>::iterator it = headers.begin();
    while(it != headers.end())
    {
        os<<it->first<<" : "<<it->second<<std::endl;
        it++;
    }

}
std::string HTTPMessage::headersAsString()
{
    std::stringstream sstr;
    std::map<std::string, std::string>::iterator it = headers.begin();
    while(it != headers.end())
    {
        sstr<<it->first<<" : "<<it->second<< "\r"  << std::endl ;
        it++;
    }
    return sstr.str();
}




/*********************************************************************************/


HTTPMessage HTTPMessage::ConnectOKMessage()
{
    HTTPMessage okResponse;
    okResponse.status_code = 200;
    okResponse.status = "Connection established";
    okResponse.http_major = 1;
    okResponse.http_minor = 1;
    okResponse.setHeader("Connection", "Close");
    return okResponse;
}

HTTPMessage HTTPMessage::ConnectFailedMessage()
{
    HTTPMessage response ;
    response.status_code = 404;
    response.status = "Failed to established connection";
    response.http_major = 1;
    response.http_minor = 1;
    response.setHeader("Connection", "Close");
    return response;
}


HTTPMessage::HTTPMessage()
{
    std::shared_ptr<Buffer> tmp(new Buffer);
    body = tmp;
//    body = std::make_shared<Buffer>(new Buffer());
}
HTTPMessage::~HTTPMessage()
{
}


bool HTTPMessage::isRequest()
{
    return (method != 0);
}

bool HTTPMessage::isHTTPS()
{
    return ( method == HTTP_CONNECT );
}
bool HTTPMessage::isHTTP()
{
    return ! isHTTPS();
}


void HTTPMessage::parseUrlForConnectRequest()
{
    std::string u = url;
    int pos = (int)u.find(":", 0);
    if( pos != std::string::npos )
    {
        size_t len = u.length();
        size_t port_start = pos + 1;
        size_t port_len = len - port_start ;
        std::string h = u.substr(0, pos - 1);
        std::string p = u.substr(port_start, port_len);
        connect_host = h;
        connect_port = stoi(p);
    }
    else
    {
        connect_host = "";
        connect_port = 0;
    }
    
}
std::string HTTPMessage::getConnectHost()
{
    parseUrlForConnectRequest();
    return connect_host;
}
int HTTPMessage::getConnectPort()
{
    parseUrlForConnectRequest();
    return connect_port;
}

std::string HTTPMessage::methodAsString()
{
    return methodStringFromEnum((enum http_method)method);
}

BufferPtr HTTPMessage::serialize()
{
//    NSString* headersString;
//    
//    if( method == 0 ){
//        headersString = [NSString stringWithFormat:@"HTTP/1.1 %ld %@",  status_code, status];
//    } else {
//        NSString* methodString = methodStringFromEnum((enum http_method)method);
//        
//        headersString = [NSString stringWithFormat:@"%@ %@ HTTP%ld/%ld\r\n", methodString, url, http_major, http_minor];
//    }
//    for (id key in headers)
//    {
//        headersString = [NSString stringWithFormat:@"%@\r\n%@ : %@", headersString, key, [headers objectForKeyedSubscript:key]];
//    }
//    headersString = [NSString stringWithFormat:@"%@\r\n\r\n", headersString];
//    NSData* headerData = [headersString  dataUsingEncoding:NSUTF8StringEncoding];
//    
//    NSMutableData* result = [[NSMutableData alloc]init];
//    
//    [result appendData:headerData ];
//    [result appendData: body];
//    
//    return result;
    return nullptr;
}
std::string HTTPMessage::firstLineAsString()
{
    std::stringstream sstr;

    std::string tmp = "";
    if( isRequest() )
    {
        sstr << methodAsString() << " HTTP/" << http_major << "." << http_minor;
    }
    else
    {
        sstr << "HTTP/" << http_major << "." << http_minor << " " << status_code << " " << status;
    }
    return sstr.str();
    
}
std::string HTTPMessage::bodyAsString()
{
    return ( *(body));
}
std::string HTTPMessage::messageAsString()
{
    std::string tmp = (firstLineAsString()) + "\r\n" + (headersAsString() + "\r\n\r\n" + *(body));
    std::cout << "\n|" << tmp << "|\n" << std::endl;
    return tmp;
}
