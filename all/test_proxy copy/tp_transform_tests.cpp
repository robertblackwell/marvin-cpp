///
/// \brief in this file we test the functions that apply the proxy transform to
/// requests going upstream to the end server and responses coming back from that server
/// and going downstream to the initial client
//
#if 1
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <catch/catch.hpp>
#include "buffer.hpp"
#include "pipe_collector.hpp"
#include "http_server.hpp"
#include "boost_stuff.hpp"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "forward_helpers.hpp"
#include "tsc_req_handler.hpp"
#include "server_runner.hpp"
#include "tp_helpers.hpp"
#include "tp_proxy_runner.hpp"
#endif 
#pragma mark - MessageBase helpers
//bool hasContentLength(MessageBase& msg)
//{
//    return (msg.hasHeader(HttpHeader::Name::ContentLength));
//}
//std::size_t getContentLength(MessageBase& msg)
//{
//    assert(msg.hasHeader(HttpHeader::Name::ContentLength));
//    int len = std::stoi(msg.getHeader(HttpHeader::Name::ContentLength));
//    return len;
//}
//
//void setContentLength(MessageBase& msg, std::size_t length)
//{
//    msg.setHeader(HttpHeader::Name::ContentLength, std::to_string(length));
//}
//void setContent(MessageBase& msg, Marvin::BufferChainSPtr content)
//{
//    msg.setBody(content);
//    setContentLength(msg, content->size());
//}
#pragma mark - mock up a MessageReader
MessageReaderSPtr makeMock()
{
    static boost::asio::io_service io;
    static TCPConnectionSPtr socketSPtr = std::make_shared<TCPConnection>(io);
    MessageReaderSPtr result = std::make_shared<MessageReader>(io, socketSPtr);
    return result;
}

//HTTP/1.1 200 OK
//Accept-Ranges: bytes
//Cache-Control: max-age=604800
//Content-Type: text/html
//Date: Sun, 24 Nov 2013 01:38:41 GMT
//Etag: "359670651"
//Expires: Sun, 01 Dec 2013 01:38:41 GMT
//Last-Modified: Fri, 09 Aug 2013 23:54:35 GMT
//Server: ECS (mia/41C4)
//X-Cache: HIT
//x-ec-custom-error: 1
//Content-Length: 1270

void fillMsgRdrAsResponse_01(MessageReaderSPtr msgRdr)
{
    msgRdr->setStatus("OK");
    msgRdr->setStatusCode(200);
    msgRdr->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionClose);
    msgRdr->setHeader("Cache-Control"," max-age=604800");
    msgRdr->setHeader("Content-Type"," text/html");
    msgRdr->setHeader("Date"," Sun, 24 Nov 2013 01:38:41 GMT");
    msgRdr->setHeader("Etag"," \"359670651\"");
    msgRdr->setHeader("Connection"," keep-alive");
    msgRdr->setHeader("Expires"," Sun, 01 Dec 2013 01:38:41 GMT");
    msgRdr->setHeader("Last-Modified"," Fri, 09 Aug 2013 23:54:35 GMT");
    msgRdr->setHeader("Server"," ECS (mia/41C4)");
    msgRdr->setHeader("X-Cache"," HIT");
    msgRdr->setHeader("x-ec-custom-error"," 1");
    msgRdr->setHeader("Transfer-Encoding"," chunk");
    std::string s = "012345678956";
    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    msgRdr->setBody(bdy);
}
void verifyResponse_01(MessageBaseSPtr msg)
{
    auto trim = [](std::string s) -> std::string {
        return boost::algorithm::trim_copy(s);
    };
    REQUIRE(msg->status() == "OK");
    REQUIRE(msg->statusCode() == 200);
    REQUIRE(msg->getHeader(HttpHeader::Name::Connection) == HttpHeader::Value::ConnectionClose);
    auto xx = msg->getHeader("Cache-Control");
    REQUIRE(msg->getHeader("Cache-Control") == trim(" max-age=604800"));
    REQUIRE(msg->getHeader("Content-Type") == trim(" text/html"));
    REQUIRE(msg->getHeader("Date") == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
    REQUIRE(msg->getHeader(HttpHeader::Name::Date) == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
    /// ETag just not passed down
    REQUIRE( ! msg->hasHeader("Etag"));
    REQUIRE( ! msg->hasHeader(HttpHeader::Name::ETag));
    /// proxy transforms chunked encoding to content-length style
    REQUIRE( ! msg->hasHeader("Transfer-Encoding"));
    REQUIRE( ! msg->hasHeader(HttpHeader::Name::TransferEncoding));
    REQUIRE(msg->hasHeader("Content-Length"));
    REQUIRE(msg->hasHeader(HttpHeader::Name::ContentLength));
    /// and we force connection close
    REQUIRE(msg->getHeader(HttpHeader::Name::Connection) == HttpHeader::Value::ConnectionClose);

    REQUIRE(msg->getHeader("Expires") == trim(" Sun, 01 Dec 2013 01:38:41 GMT"));
    REQUIRE(msg->getHeader("Last-Modified") == trim(" Fri, 09 Aug 2013 23:54:35 GMT"));
    REQUIRE(msg->getHeader("Server") == trim(" ECS (mia/41C4)"));
    REQUIRE(msg->getHeader("X-Cache") == trim(" HIT"));
    REQUIRE(msg->getHeader("x-ec-custom-error") == trim(" 1"));
//    std::string s = "012345678956";
//    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
//    msgRdr->setBody(bdy);

}
#pragma mark - sample filling of upstream request

void fillMsgRdrAsRequest02(MessageReaderSPtr msgRdr)
{
// GET / HTTPS/1.1
// Host: example.org
// User-Agent: Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50
// Accept: text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1
// Accept-Language: en
// Accept-Charset: iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1
// Accept-Encoding: deflate, gzip, x-gzip, identity, *;q=0
// Connection: Keep-Alive, TE
//TE: deflate, gzip, chunked, identity, trailers
    msgRdr->setMethod(HTTP_POST);
    // note requests through a proxy must provide absolute uri on the first line
    // proxy mat turn that into a relative url
    msgRdr->setUri("http://example.org/somepath/script.php?parm=123456#fragment");
    msgRdr->setHeader(HttpHeader::Name::Host, "example.org");
    msgRdr->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msgRdr->setHeader("Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msgRdr->setHeader("Accept-Language","en");
    msgRdr->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msgRdr->setHeader(HttpHeader::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
    msgRdr->setHeader(HttpHeader::Name::Connection,"Keep-Alive, TE");
    msgRdr->setHeader("TE","deflate, gzip, chunked, trailer");
    msgRdr->setHeader(HttpHeader::Name::TransferEncoding,"chunked");
    msgRdr->setHeader(HttpHeader::Name::ETag,"1928273tefadseercnbdh");
    std::string s = "012345678956";
    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    setContent(*msgRdr, bdy);
}
void verifyRequest_02(MessageBaseSPtr msgSPtr)
{
    REQUIRE(msgSPtr->uri() == "/somepath/script.php?parm=123456#fragment");
    REQUIRE(msgSPtr->getHeader(HttpHeader::Name::Host) == "example.org:80" );
    REQUIRE(msgSPtr->getHeader(HttpHeader::Name::AcceptEncoding) == "identity");
    REQUIRE(msgSPtr->getHeader(HttpHeader::Name::Connection) == HttpHeader::Value::ConnectionClose);
    REQUIRE(msgSPtr->getHeader(HttpHeader::Name::TE) == "");
    REQUIRE(msgSPtr->getHeader("User-Agent") =="Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    REQUIRE(msgSPtr->getHeader("Accept-Language") == "en");
    REQUIRE(msgSPtr->getHeader("Accept-Charset") == "iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    REQUIRE( ! msgSPtr->hasHeader(HttpHeader::Name::TransferEncoding));
    REQUIRE( ! msgSPtr->hasHeader(HttpHeader::Name::ETag));
}
#pragma mark - verify minimum requirements for a request
void fillMsgRdrAsRequest(MessageReaderSPtr msgRdr)
{
    msgRdr->setMethod(HTTP_POST);
    helpers::fillRequestFromUri(*msgRdr, "http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    msgRdr->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionKeepAlive);
    std::string s = "012345678956";
    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    setContent(*msgRdr, bdy);
}
bool verifyRequest_MimimumRequirements(MessageBaseSPtr msgSPtr)
{
    auto meth = msgSPtr->getMethodAsString();
    REQUIRE_FALSE(meth == "");
//    if (meth == "") return false;
    auto uri = msgSPtr->uri();
    REQUIRE_FALSE(uri == "");
//    if (uri == "") return false;
    REQUIRE( msgSPtr->hasHeader(HttpHeader::Name::Host));
//    if( ! msgSPtr->hasHeader(HttpHeader::Name::Host)) return false;
    REQUIRE( msgSPtr->hasHeader(HttpHeader::Name::Connection));
    {
    auto bb = ( (msgSPtr->hasHeader(HttpHeader::Name::ContentLength)) || (msgSPtr->hasHeader(HttpHeader::Name::TransferEncoding)));
    REQUIRE(bb);
    }
//    if( ! msgSPtr->hasHeader(HttpHeader::Name::ContentLength)
//        || (msgSPtr->hasHeader(HttpHeader::Name::TransferEncoding))) return false;
    if(msgSPtr->hasHeader(HttpHeader::Name::ContentLength) && (msgSPtr->getHeader(HttpHeader::Name::ContentLength) != "0" )){
        int cl = std::stoi(msgSPtr->getHeader(HttpHeader::Name::ContentLength));
        auto contentChain = msgSPtr->getBody();
        REQUIRE(contentChain != nullptr);
        if( contentChain != nullptr) {
            REQUIRE(contentChain->size() == cl);
        }
    }
    return true;
}
#pragma mark - TEST
TEST_CASE("Helpers_Example", "[example]")
{
    MessageReaderSPtr msg = makeMock();
    fillMsgRdrAsRequest(msg);
    verifyRequest_MimimumRequirements(msg);
//    std::cout << msg->str() << std::endl;
}
TEST_CASE("Helpers_downstream01", "[downstream01]")
{
    MessageReaderSPtr msgRdr = makeMock();
    fillMsgRdrAsResponse_01(msgRdr);
//    verifyRequest_MimimumRequirements(msgRdr);
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    Marvin::ErrorType err = Marvin::make_error_ok();
    helpers::makeDownstreamResponse(msgSPtr,msgRdr, err);
    verifyResponse_01(msgSPtr);
//    std::cout << msgSPtr->str() << std::endl;
}
TEST_CASE("Helpers_upstream02", "[upstream02]")
{
    MessageReaderSPtr msgRdr = makeMock();
    fillMsgRdrAsRequest02(msgRdr);
    verifyRequest_MimimumRequirements(msgRdr);
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    helpers::makeUpstreamRequest(msgSPtr, msgRdr);
    verifyRequest_02(msgSPtr);
//    std::cout << msgSPtr->str() << std::endl;
}

