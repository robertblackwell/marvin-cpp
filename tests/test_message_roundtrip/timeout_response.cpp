#include "timeout_response.hpp"

#include <doctest/doctest.h>

#include <boost/algorithm/string.hpp>
#include <json/json.hpp>

#include <marvin/buffer/buffer_chain.hpp>

using namespace Marvin;

TimeoutResponse::TimeoutResponse(
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
std::string TimeoutResponse::getHost() {return m_host;}
std::string TimeoutResponse::getPort() {return m_port;}
void TimeoutResponse::verifyResponse(ErrorType& er, MessageBaseSPtr response)
{
    namespace bs = boost::system;
    auto ec_cancelled = bs::error_code(bs::errc::operation_canceled, bs::system_category());
    std::cout << __PRETTY_FUNCTION__ << " : " << er.message() << std::endl;
    std::cout << __PRETTY_FUNCTION__ << " : " << ec_cancelled.message() << std::endl;
    CHECK(er);
    CHECK(er == ec_cancelled);
    CHECK(er.message() == "Operation canceled");
    std::cout << __PRETTY_FUNCTION__ << " : " << er.message() << std::endl;
}
BufferChainSPtr TimeoutResponse::makeBody()
{
    using namespace Marvin;
    BufferChainSPtr chain_sptr  = BufferChain::makeSPtr(m_body);
    return chain_sptr;
}
MessageBaseSPtr TimeoutResponse::makeRequest()
{
    /// this sends the request to our mitm proxy
    MessageBaseSPtr msg = std::make_shared<MessageBase>();
    msg->setMethod(HTTP_POST);
    msg->setUri(m_path);
    msg->setHeader(HeadersV2::Host, m_host);
    msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msg->setHeader(
        "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msg->setHeader("Accept-Language","en");
    msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msg->setHeader(HeadersV2::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");

    msg->setHeader(HeadersV2::Connection,"Close");
    msg->setHeader(HeadersV2::ETag,"1928273tefadseercnbdh");
    msg->setHeader("X-SPECIAL-HEADER", "proof of passthru");
    // std::string s = "012345678956";
    // BufferChainSPtr bdy = BufferChain::makeSPtr(s);
    // msg->setContent(bdy);
    m_request_sptr = msg;
    return msg;
}
