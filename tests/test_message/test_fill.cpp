///
/// This file tests fill in the fields in a MesasgeBase and/or MessageReader
///
/// Coincidentally some testing of the Uri() class also takes place.
/// 
/// No actual transmission is involved
//
#if 1
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>

#include <marvin/buffer/buffer.hpp>

#include <marvin/http/uri.hpp>
#include <marvin/helpers/mitm.hpp>
#include <marvin/connection/socket_factory.hpp>

#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)

#endif 
#pragma mark - mock up a MessageReader

using namespace Marvin;
namespace {

/// Create a MessageReaderSPtr to be filled during various tests
MessageReaderSPtr makeMock()
{
    static boost::asio::io_service io;
    static ISocketSPtr socketSPtr = socketFactory(io);
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

/// Make a typical response message
void fillMsgRdrAsResponse_01(MessageReaderSPtr msgRdr)
{
    msgRdr->setStatus("OK");
    msgRdr->setStatusCode(200);
    msgRdr->setHeader(HeadersV2::Connection, HeadersV2::ConnectionClose);
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
    msgRdr->setContentBuffer(bdy);
}
/// Verify the correctness of the typical response message
void verifyResponse_01(MessageBaseSPtr msg)
{
    auto trim = [](std::string s) -> std::string {
        return boost::algorithm::trim_copy(s);
    };
    REQUIRE(msg->status() == "OK");
    REQUIRE(msg->statusCode() == 200);
    REQUIRE(msg->getHeader(HeadersV2::Connection) == HeadersV2::ConnectionClose);
    auto xx = msg->getHeader("Cache-Control");
    REQUIRE(msg->getHeader("Cache-Control") == trim(" max-age=604800"));
    REQUIRE(msg->getHeader("Content-Type") == trim(" text/html"));
    REQUIRE(msg->getHeader("Date") == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
    REQUIRE(msg->getHeader(HeadersV2::Date) == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
    /// ETag just not passed down
    REQUIRE( ! msg->hasHeader("Etag"));
    REQUIRE( ! msg->hasHeader(HeadersV2::ETag));
    /// proxy transforms chunked encoding to content-length style
    REQUIRE( ! msg->hasHeader("Transfer-Encoding"));
    REQUIRE( ! msg->hasHeader(HeadersV2::TransferEncoding));
    REQUIRE(msg->hasHeader("Content-Length"));
    REQUIRE(msg->hasHeader(HeadersV2::ContentLength));
    /// and we force connection close
    REQUIRE(msg->getHeader(HeadersV2::Connection) == HeadersV2::ConnectionClose);

    REQUIRE(msg->getHeader("Expires") == trim(" Sun, 01 Dec 2013 01:38:41 GMT"));
    REQUIRE(msg->getHeader("Last-Modified") == trim(" Fri, 09 Aug 2013 23:54:35 GMT"));
    REQUIRE(msg->getHeader("Server") == trim(" ECS (mia/41C4)"));
    REQUIRE(msg->getHeader("X-Cache") == trim(" HIT"));
    REQUIRE(msg->getHeader("x-ec-custom-error") == trim(" 1"));
//    std::string s = "012345678956";
//    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
//    msgRdr->setBody(bdy);

}
/// Fill a typical pstream non proxy request
void fillMsgRdrAsRequest_01(MessageReaderSPtr msgRdr)
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
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // proxy absolute uri
    Helpers::applyUriProxy(msgRdr, uri);
//    msgRdr->setUri("http://example.org/somepath/script.php?parm=123456#fragment");
//    msgRdr->setHeader(HeadersV2::Host, "example.org");
    msgRdr->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msgRdr->setHeader("Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msgRdr->setHeader("Accept-Language","en");
    msgRdr->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msgRdr->setHeader(HeadersV2::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
    msgRdr->setHeader(HeadersV2::Connection,"Keep-Alive, TE");
    msgRdr->setHeader("TE","deflate, gzip, chunked, trailer");
    msgRdr->setHeader(HeadersV2::TransferEncoding,"chunked");
    msgRdr->setHeader(HeadersV2::ETag,"1928273tefadseercnbdh");
    std::string s = "012345678956";
    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    msgRdr->setContent(bdy);
}
/// Verify the typical non proxy request
void verifyRequest_01(MessageBaseSPtr msgSPtr)
{
    /// relative url
    REQUIRE(msgSPtr->uri() == "/somepath/script.php?parm=123456#fragment");
    /// host name has port
    REQUIRE(msgSPtr->getHeader(HeadersV2::Host) == "example.org:9999" );
    REQUIRE(msgSPtr->getHeader(HeadersV2::AcceptEncoding) == "identity");
    REQUIRE(msgSPtr->getHeader(HeadersV2::Connection) == HeadersV2::ConnectionClose);
    REQUIRE(msgSPtr->getHeader(HeadersV2::TE) == "");
    REQUIRE(msgSPtr->getHeader("User-Agent") =="Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    REQUIRE(msgSPtr->getHeader("Accept-Language") == "en");
    REQUIRE(msgSPtr->getHeader("Accept-Charset") == "iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    REQUIRE( ! msgSPtr->hasHeader(HeadersV2::TransferEncoding));
    REQUIRE( ! msgSPtr->hasHeader(HeadersV2::ETag));
}
/// request 02 test a proxy request
void fillMsgRequest_02(MessageBaseSPtr msgSPtr)
{
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // proxy full uri
    Helpers::applyUriProxy(msgSPtr, uri);
}
/// verify request 02 test a proxy request
void verifyRequest_02(MessageBaseSPtr msgSPtr)
{
    REQUIRE(msgSPtr->uri() == "http://example.org:9999/somepath/script.php?parm=123456#fragment");
    REQUIRE(msgSPtr->getHeader(HeadersV2::Host) == "example.org:9999" );
}
/// Request 03 non proxy request
void fillMsgRequest03(MessageBaseSPtr msgSPtr)
{
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // non proxy relative uri
    Helpers::applyUriNonProxy(msgSPtr, uri);
}
/// Verify request 03
void verifyRequest_03(MessageBaseSPtr msgSPtr)
{
    REQUIRE(msgSPtr->uri() == "/somepath/script.php?parm=123456#fragment");
    REQUIRE(msgSPtr->getHeader(HeadersV2::Host) == "example.org:9999" );
}
/// Fill minimum requirements for a request
void fillMsgRdrAsRequest(MessageReaderSPtr msgRdr)
{
    msgRdr->setMethod(HTTP_POST);
    Marvin::Uri uri("http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    Helpers::applyUriProxy(msgRdr, uri);
//    Helpers::fillRequestFromUri(*msgRdr, "http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    msgRdr->setHeader(HeadersV2::Connection, HeadersV2::ConnectionKeepAlive);
    std::string s = "012345678956";
    Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    msgRdr->setContent(bdy);
}
/// Verify minimum requirements
bool verifyRequest_MimimumRequirements(MessageBaseSPtr msgSPtr)
{
    auto meth = msgSPtr->getMethodAsString();
    CHECK_FALSE(meth == "");
    auto uri = msgSPtr->uri();
    CHECK_FALSE(uri == "");
    CHECK( msgSPtr->hasHeader(HeadersV2::Host));
    CHECK( msgSPtr->hasHeader(HeadersV2::Connection));
    {
    auto bb = ( (msgSPtr->hasHeader(HeadersV2::ContentLength)) || (msgSPtr->hasHeader(HeadersV2::TransferEncoding)));
    CHECK(bb);
    }
    if(msgSPtr->hasHeader(HeadersV2::ContentLength) && (msgSPtr->getHeader(HeadersV2::ContentLength) != "0" )){
        int cl = std::stoi(msgSPtr->getHeader(HeadersV2::ContentLength));
        auto contentChain = msgSPtr->getContentBuffer();
        CHECK(contentChain != nullptr);
        if( contentChain != nullptr) {
            CHECK(contentChain->size() == cl);
        }
    }
    return true;
}
} //namespace
#pragma mark - TEST_CASE 
TEST_CASE("Helpers_Example")
{
    Marvin::Uri u("http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    CHECK(u.scheme() == "http");
    CHECK(u.server() == "somewhere.com");
    CHECK(u.host() == "somewhere.com:80");
    CHECK(u.port() == 80);
    CHECK(u.search() == "a=1111#fragment");
    CHECK(u.relativePath() == "/subdirpath/index.php?a=1111#fragment");
    CHECK(u.absolutePath() == "http://somewhere.com:80/subdirpath/index.php?a=1111#fragment");
}
TEST_CASE("Uri")
{
    MessageReaderSPtr msg = makeMock();
    fillMsgRdrAsRequest(msg);
    verifyRequest_MimimumRequirements(msg);
}
TEST_CASE("response_01")
{
    MessageReaderSPtr msgRdr = makeMock();
    fillMsgRdrAsResponse_01(msgRdr);
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    Marvin::ErrorType err = Marvin::make_error_ok();
    Helpers::makeDownstreamResponse(msgSPtr,msgRdr, err);
    verifyResponse_01(msgSPtr);
}
TEST_CASE("request_01")
{
    MessageReaderSPtr msgRdr = makeMock();
    fillMsgRdrAsRequest_01(msgRdr);
    verifyRequest_MimimumRequirements(msgRdr);
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    Helpers::makeUpstreamRequest(msgSPtr, msgRdr);
    verifyRequest_01(msgSPtr);
}
TEST_CASE("request_02")
{
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_02(msgSPtr);
    verifyRequest_02(msgSPtr);
}

TEST_CASE("request_03")
{
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest03(msgSPtr);
    verifyRequest_03(msgSPtr);
}
TEST_CASE("min_requirement_request_01")
{
    MessageReaderSPtr msgRdr = makeMock();
    fillMsgRdrAsRequest_01(msgRdr);
    verifyRequest_MimimumRequirements(msgRdr);
    MessageBaseSPtr msgSPtr = std::make_shared<MessageBase>();
    std::cout << traceMessage(*msgSPtr) << std::endl;
    std::cout << *msgSPtr << std::endl;
}

