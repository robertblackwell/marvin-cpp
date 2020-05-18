#include "any_response.hpp"

#include <doctest/doctest.h>

#include <boost/algorithm/string.hpp>
#include <json/json.hpp>

#include <marvin/buffer/buffer.hpp>

using namespace Marvin;

AnyResponse::AnyResponse(
            std::string path,   // the string that goes after the method usually for non proxy requests a relative path like /echo/smart
            HttpMethod  method,
            std::string scheme, // http or https
            std::string host,   // host name without the port so localhost not localhost:3000
            std::string port,    // port such as 3000){}
            std::string body 
)           
{
    m_path = path;
    m_scheme = scheme;
    m_method = method;
    m_host = host;
    m_port = port;
    m_body = body;
}
std::string AnyResponse::getHost() {return m_host;}
std::string AnyResponse::getPort() {return m_port;}
void AnyResponse::verifyResponse(ErrorType& er, MessageBaseSPtr response)
{
    CHECK(!er);
    std::cout << __PRETTY_FUNCTION__ << " : " << er.message() << std::endl;
    BufferChainSPtr bsp = response->getContentBuffer();
    std::string raw_body = bsp->to_string();
    CHECK(response->status_code() == 200);
}
BufferChainSPtr AnyResponse::makeBody()
{
    using namespace Marvin;
    BufferChainSPtr chain_sptr  = BufferChain::makeSPtr(m_body);
    return chain_sptr;
}
MessageBaseSPtr AnyResponse::makeRequest()
{
    /// this sends the request to our mitm proxy
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    msg->method(HTTP_POST);
    msg->target(m_path);
    msg->header(HeadersV2::Host, m_host);
    msg->header("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msg->header(
        "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msg->header("Accept-Language","en");
    msg->header("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msg->header(HeadersV2::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");

    msg->header(HeadersV2::Connection,"Close");
    msg->header(HeadersV2::ETag,"1928273tefadseercnbdh");
    msg->header("X-SPECIAL-HEADER", "proof of passthru");
    // std::string s = "012345678956";
    // BufferChainSPtr bdy = BufferChain::makeSPtr(s);
    // msg->setContent(bdy);
    m_request_sptr = msg;
    return msg;
}
