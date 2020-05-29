///
/// This file tests fill in the fields in a MesasgeBase and/or MessageReader
///
/// Coincidentally some testing of the Uri() class also takes place.
/// 
/// No actual transmission is involved
//
#if 1

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <boost/algorithm/string/trim.hpp>
#include <doctest/doctest.h>
#include <boost/optional/optional_io.hpp>
#include <marvin/boost_stuff.hpp>

#include <marvin/buffer/buffer.hpp>

#include <marvin/http/uri.hpp>
#include <marvin/helpers/mitm.hpp>
#include <marvin/connection/socket_factory.hpp>

#endif
#pragma mark - mock up a MessageReader

using namespace Marvin;
namespace {

/// Create a MessageBase::SPtr to be filled during various tests
MessageBase::SPtr makeMock()
{
    MessageBase::SPtr result = std::make_shared<MessageBase>();
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
void fillMsgAsResponse_01(MessageBase::SPtr msgRdr)
{
    msgRdr->reason("OK");
    msgRdr->status_code(200);
    msgRdr->header(HeaderFields::Connection, HeaderFields::ConnectionClose);
    msgRdr->header("Cache-Control", " max-age=604800");
    msgRdr->header("Content-Type", " text/html");
    msgRdr->header("Date", " Sun, 24 Nov 2013 01:38:41 GMT");
    msgRdr->header("Etag", " \"359670651\"");
    msgRdr->header("Connection", " keep-alive");
    msgRdr->header("Expires", " Sun, 01 Dec 2013 01:38:41 GMT");
    msgRdr->header("Last-Modified", " Fri, 09 Aug 2013 23:54:35 GMT");
    msgRdr->header("Server", " ECS (mia/41C4)");
    msgRdr->header("X-Cache", " HIT");
    msgRdr->header("x-ec-custom-error", " 1");
    msgRdr->header("Transfer-Encoding", " chunk");
    std::string s = "012345678956";
    Marvin::BufferChain::SPtr bdy = Marvin::makeBufferChainSPtr(s);
    msgRdr->set_body_buffer_chain(bdy);
}

/// Verify the correctness of the typical response message
void verifyResponse_01(MessageBase::SPtr msg)
{
    auto trim = [](std::string s) -> std::string
    {
        return boost::algorithm::trim_copy(s);
    };
        REQUIRE(msg->reason() == "OK");
        REQUIRE(msg->status_code() == 200);
        REQUIRE(msg->header(HeaderFields::Connection).get() == HeaderFields::ConnectionClose);
    auto xx = msg->header("Cache-Control").get();
        REQUIRE(msg->header("Cache-Control").get() == trim(" max-age=604800"));
        REQUIRE(msg->header("Content-Type").get() == trim(" text/html"));
        REQUIRE(msg->header("Date").get() == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
        REQUIRE(msg->header(HeaderFields::Date).get() == trim(" Sun, 24 Nov 2013 01:38:41 GMT"));
    /// ETag just not passed down
        REQUIRE(!msg->header("Etag"));
        REQUIRE(!msg->header(HeaderFields::ETag));
    /// proxy transforms chunked encoding to content-length style
        REQUIRE(!msg->header("Transfer-Encoding"));
        REQUIRE(!msg->header(HeaderFields::TransferEncoding));
        REQUIRE(msg->header("Content-Length"));
        REQUIRE(msg->header(HeaderFields::ContentLength));
    /// and we force connection close
        REQUIRE(msg->header(HeaderFields::Connection).get() == HeaderFields::ConnectionClose);

        REQUIRE(msg->header("Expires").get() == trim(" Sun, 01 Dec 2013 01:38:41 GMT"));
        REQUIRE(msg->header("Last-Modified").get() == trim(" Fri, 09 Aug 2013 23:54:35 GMT"));
        REQUIRE(msg->header("Server").get() == trim(" ECS (mia/41C4)"));
        REQUIRE(msg->header("X-Cache").get() == trim(" HIT"));
        REQUIRE(msg->header("x-ec-custom-error").get() == trim(" 1"));
//    std::string s = "012345678956";
//    Marvin::BufferChain::SPtr bdy = Marvin::makeBufferChainSPtr(s);
//    msgRdr->setBody(bdy);

}

/// Fill a typical pstream non proxy request
void fillMsgAsRequest_01(MessageBase::SPtr msgRdr)
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
    msgRdr->method(HTTP_POST);
    // note requests through a proxy must provide absolute uri on the first line
    // proxy mat turn that into a relative url
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // proxy absolute uri
    Helpers::apply_uri_proxy(msgRdr, uri);
//    msgRdr->target("http://example.org/somepath/script.php?parm=123456#fragment");
//    msgRdr->header(HeaderFields::Host, "example.org");
    msgRdr->header("User-Agent", "Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msgRdr->header("Accept",
                      "text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msgRdr->header("Accept-Language", "en");
    msgRdr->header("Accept-Charset", "iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msgRdr->header(HeaderFields::AcceptEncoding, "deflate, gzip, x-gzip, identity, *;q=0");
    msgRdr->header(HeaderFields::Connection, "Keep-Alive, TE");
    msgRdr->header("TE", "deflate, gzip, chunked, trailer");
    msgRdr->header(HeaderFields::TransferEncoding, "chunked");
    msgRdr->header(HeaderFields::ETag, "1928273tefadseercnbdh");
    std::string s = "012345678956";
    Marvin::BufferChain::SPtr bdy = Marvin::makeBufferChainSPtr(s);
    msgRdr->set_body(bdy);
}

/// Verify the typical non proxy request
void verifyRequest_01(MessageBase::SPtr msgSPtr)
{
    /// relative url
        REQUIRE(msgSPtr->target() == "/somepath/script.php?parm=123456#fragment");
    /// host name has port
        REQUIRE(msgSPtr->header(HeaderFields::Host).get() == "example.org:9999");
        REQUIRE(msgSPtr->header(HeaderFields::AcceptEncoding).get() == "identity");
        REQUIRE(msgSPtr->header(HeaderFields::Connection).get() == HeaderFields::ConnectionClose);
        REQUIRE(msgSPtr->header(HeaderFields::TE).get() == "");
        REQUIRE(msgSPtr->header("User-Agent").get() ==
                "Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        REQUIRE(msgSPtr->header("Accept-Language").get() == "en");
        REQUIRE(msgSPtr->header("Accept-Charset").get() == "iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        REQUIRE(!msgSPtr->header(HeaderFields::TransferEncoding));
        REQUIRE(!msgSPtr->header(HeaderFields::ETag));
}

/// request 02 test a proxy request
void fillMsgRequest_02(MessageBase::SPtr msgSPtr)
{
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // proxy full uri
    Helpers::apply_uri_proxy(msgSPtr, uri);
}

/// verify request 02 test a proxy request
void verifyRequest_02(MessageBase::SPtr msgSPtr)
{
        REQUIRE(msgSPtr->target() == "http://example.org:9999/somepath/script.php?parm=123456#fragment");
        REQUIRE(msgSPtr->header(HeaderFields::Host).get() == "example.org:9999");
}

/// Request 03 non proxy request
void fillMsgRequest_03(MessageBase::SPtr msgSPtr)
{
    Marvin::Uri uri("http://example.org:9999/somepath/script.php?parm=123456#fragment");
    // non proxy relative uri
    Helpers::apply_uri_non_proxy(msgSPtr, uri);
}

/// Verify request 03
void verifyRequest_03(MessageBase::SPtr msgSPtr)
{
        REQUIRE(msgSPtr->target() == "/somepath/script.php?parm=123456#fragment");
        REQUIRE(msgSPtr->header(HeaderFields::Host).get() == "example.org:9999");
}

/// Verify request 03 using reference not pointer
void verifyNonPointerRequest_03(MessageBase &msg)
{
        REQUIRE(msg.target() == "/somepath/script.php?parm=123456#fragment");
        REQUIRE(msg.header(HeaderFields::Host).get() == "example.org:9999");
}

/// Verify request 03 using reference not pointer
void failedNonPointerRequest_03(MessageBase &msg)
{
        REQUIRE(msg.target() != "/somepath/script.php?parm=123456#fragment");
        REQUIRE(!msg.header(HeaderFields::Host));
}

/// Fill minimum requirements for a request
void fillMsgAsRequest(MessageBase::SPtr msgRdr)
{
    msgRdr->method(HTTP_POST);
    Marvin::Uri uri("http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    Helpers::apply_uri_proxy(msgRdr, uri);
//    Helpers::fillRequestFromUri(*msgRdr, "http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
    msgRdr->header(HeaderFields::Connection, HeaderFields::ConnectionKeepAlive);
    std::string s = "012345678956";
    Marvin::BufferChain::SPtr bdy = Marvin::makeBufferChainSPtr(s);
    msgRdr->set_body(bdy);
}

/// Verify minimum requirements
bool verifyRequest_MimimumRequirements(MessageBase::SPtr msgSPtr)
{
    auto meth = msgSPtr->method_string();
        CHECK_FALSE(meth == "");
    auto uri = msgSPtr->target();
        CHECK_FALSE(uri == "");
        CHECK(msgSPtr->header(HeaderFields::Host));
        CHECK(msgSPtr->header(HeaderFields::Connection));
    {
        auto bb = ((msgSPtr->header(HeaderFields::ContentLength)) || (msgSPtr->header(HeaderFields::TransferEncoding)));
            CHECK(bb);
    }
    auto hopt = msgSPtr->header(HeaderFields::ContentLength);
    if ((!!hopt) && (hopt.get() != "0")) {
        int cl = std::stoi(hopt.get());
        auto contentChain = msgSPtr->get_body_buffer_chain();
            CHECK(contentChain != nullptr);
        if (contentChain != nullptr) {
                CHECK(contentChain->size() == cl);
        }
    }
    return true;
}
} //namespace
#pragma mark - TEST_CASE
TEST_CASE ("Helpers_Example")
{
    Marvin::Uri u("http://username:password@somewhere.com/subdirpath/index.php?a=1111#fragment");
        CHECK(u.scheme() == "http");
        CHECK(u.server() == "somewhere.com");
        CHECK(u.host_no_port() == "somewhere.com");
        CHECK(u.host() == "somewhere.com:80");
        CHECK(u.host_and_port() == "somewhere.com:80");
        CHECK(u.port() == 80);
        CHECK(u.search() == "a=1111#fragment");
        CHECK(u.relativePath() == "/subdirpath/index.php?a=1111#fragment");
        CHECK(u.absolutePath() == "http://somewhere.com:80/subdirpath/index.php?a=1111#fragment");
}

TEST_CASE ("Uri")
{
    MessageBase::SPtr msg = makeMock();
    fillMsgAsRequest(msg);
    verifyRequest_MimimumRequirements(msg);
}

TEST_CASE ("response_01")
{
    MessageBase::SPtr msgRdr = makeMock();
    fillMsgAsResponse_01(msgRdr);
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    Marvin::ErrorType err = Marvin::make_error_ok();
    Helpers::make_downstream_response(msgSPtr, msgRdr, err);
    verifyResponse_01(msgSPtr);
}

TEST_CASE ("request_01")
{
    MessageBase::SPtr msgRdr = makeMock();
    fillMsgAsRequest_01(msgRdr);
    verifyRequest_MimimumRequirements(msgRdr);
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    Helpers::make_upstream_request(msgSPtr, msgRdr);
    verifyRequest_01(msgSPtr);
}

TEST_CASE ("request_02")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_02(msgSPtr);
    verifyRequest_02(msgSPtr);
}

TEST_CASE ("request_03")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_03(msgSPtr);
    verifyRequest_03(msgSPtr);
}

TEST_CASE ("min_requirement_request_01")
{
    MessageBase::SPtr msgRdr = makeMock();
    fillMsgAsRequest_01(msgRdr);
    verifyRequest_MimimumRequirements(msgRdr);
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    std::cout << trace_message(*msgSPtr) << std::endl;
    std::cout << *msgSPtr << std::endl;
}

TEST_CASE ("copy ctor")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_03(msgSPtr);
    msgSPtr->get_body_buffer_chain()->append("01234567890");
    // make a copy
    MessageBase tmp_msg{(*msgSPtr)};
    // demonstrate we have two copies of the same value
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == tmp_msg.get_body_buffer_chain()->to_string());
        CHECK(msgSPtr->get_body_buffer_chain()->size() == tmp_msg.get_body_buffer_chain()->size());
    // now prove the body buffers have been copied and not just two pointers to the same thing
    tmp_msg.get_body_buffer_chain()->append("XXXX");
        CHECK(tmp_msg.get_body_buffer_chain()->to_string() == "01234567890XXXX");
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == "01234567890");

    verifyRequest_03(msgSPtr);
    verifyNonPointerRequest_03(tmp_msg);
    std::string s4 = msgSPtr->get_body_buffer_chain()->block_at(0).toString();
}

TEST_CASE ("copy assignment")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_03(msgSPtr);
    msgSPtr->get_body_buffer_chain()->append("01234567890");
    // make a new one and assign
    MessageBase tmp_msg;
    tmp_msg = *msgSPtr;

    // demonstrate we have two copies of the same value
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == tmp_msg.get_body_buffer_chain()->to_string());
        CHECK(msgSPtr->get_body_buffer_chain()->size() == tmp_msg.get_body_buffer_chain()->size());
    // now prove the body buffers have been copied and not just two pointers to the same thing
    tmp_msg.get_body_buffer_chain()->append("XXXX");
        CHECK(tmp_msg.get_body_buffer_chain()->to_string() == "01234567890XXXX");
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == "01234567890");

    verifyRequest_03(msgSPtr);
    verifyNonPointerRequest_03(tmp_msg);
    std::string s4 = msgSPtr->get_body_buffer_chain()->block_at(0).toString();
}

TEST_CASE ("move ctor")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_03(msgSPtr);
    msgSPtr->get_body_buffer_chain()->append("01234567890");
    // ctor move
    MessageBase tmp_msg{std::move(*msgSPtr)};
    // demonstrate that it was a move
        CHECK(tmp_msg.get_body_buffer_chain()->to_string() == "01234567890");
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == "");
        CHECK(msgSPtr->get_body_buffer_chain()->size() == 0);
    failedNonPointerRequest_03(*msgSPtr);
    verifyNonPointerRequest_03(tmp_msg);
}

TEST_CASE ("move assignment")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgRequest_03(msgSPtr);
    msgSPtr->get_body_buffer_chain()->append("01234567890");
    // ctor move
    MessageBase tmp_msg;
    tmp_msg = std::move(*msgSPtr);
    // demonstrate that it was a move
        CHECK(tmp_msg.get_body_buffer_chain()->to_string() == "01234567890");
        CHECK(msgSPtr->get_body_buffer_chain()->to_string() == "");
        CHECK(msgSPtr->get_body_buffer_chain()->size() == 0);
    failedNonPointerRequest_03(*msgSPtr);
    verifyNonPointerRequest_03(tmp_msg);
}
TEST_CASE("serialize 1")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 10000; i++) {
        ContigBuffer::SPtr mb = serialize_headers(*msgSPtr);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::cout << "serialize 1" << " duration(millisecs): " << duration.count() << std::endl;
}
//TEST_CASE("serialize 2")
//{
//    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
//    fillMsgAsResponse_01(msgSPtr);
//    auto start = std::chrono::high_resolution_clock::now();
//    for(int i = 0; i < 10000; i++) {
//        BufferChain::SPtr bc_sptr = makeBufferChainSPtr();
//        serialize_headers(*msgSPtr, bc_sptr);
//    }
//    auto stop = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//    auto x = duration.count();
//    std::cout << "serialize 2" << " duration(millisecs): " << duration.count() << std::endl;
//}
TEST_CASE("header_serialize 1")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 10000; i++)
    {
        std::string str = "";
        std::ostringstream os;
        os.str(str);
        for (auto const &h : msgSPtr->headers()) {
            os << h.key << ": " << h.value << "\r\n";
        }
        // end of headers
        os << "\r\n";
        str = os.str();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::cout << "headers serialize 1" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 1" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("header_serialize_2")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 10000; i++)
    {
        BufferChain::SPtr bchain_sptr = makeBufferChainSPtr();
        ContigBuffer::SPtr mb2 = makeContigBufferSPtr(256);
        auto hdrs = msgSPtr->headers();
        for(auto const& h : hdrs) {
            int num_bytes = 0;
            do {
                mb2 = makeContigBufferSPtr(256 + num_bytes + 10);
                num_bytes = snprintf((char *) mb2->data(), mb2->capacity(), "%s: %s\r\n", h.key.c_str(), h.value.c_str());
                mb2->setSize(num_bytes);
            } while(num_bytes > 256 - 1);
            bchain_sptr->push_back(mb2);
        }
        bchain_sptr->append("\r\n");
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::cout << "headers serialize 2" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 2" << " duration(millisecs): " << duration.count() << std::endl;
}
std::string fmt_headers_stream(MessageBase::SPtr msgSPtr)
{
    std::string str = "";
    std::ostringstream os;
    os.str(str);
    auto hdrs = msgSPtr->headers();
    for (auto const &h : hdrs) {
        os << h.key << ": " << h.value << "\r\n";
    }
// end of headers
    os << "\r\n";
    return os.str();
}
void my_memcpy_string(ContigBuffer::SPtr mb, std::string const& str)
{
    memcpy(mb->nextAvailable(), str.c_str(), str.size()); mb->setSize(mb->size()+str.size());
}
void fmtHeaders2(Marvin::HeaderFields& hdrs, ContigBuffer::SPtr mb)
{
    static const std::string lfcr = "\r\n";
    static const std::string colon{": "};
    for(auto const& h : hdrs) {
        my_memcpy_string(mb, h.key);
        my_memcpy_string(mb, colon);
        my_memcpy_string(mb, h.value);
        my_memcpy_string(mb, lfcr);
    }
    my_memcpy_string(mb, lfcr);
}

void fmtHeaders1(Marvin::HeaderFields& hdrs, ContigBuffer::SPtr mb)
{
    for(auto const& h : hdrs) {
        memcpy(mb->nextAvailable(), h.key.c_str(), h.key.size());
        mb->setSize(mb->size()+h.key.size());
        memcpy(mb->nextAvailable(), (char*)": ", 2);
        mb->setSize(mb->size() + 2);
        memcpy(mb->nextAvailable(), h.value.c_str(), h.value.size());
        mb->setSize(mb->size() + h.value.size());
        memcpy(mb->nextAvailable(), (char*)"\r\n", 2);
        mb->setSize(mb->size() + 2);
    }
    memcpy(mb->nextAvailable(), (char*)"\r\n", 2); mb->setSize(mb->size() + 2);
}

void fmtHeaders3(Marvin::HeaderFields& hdrs, ContigBuffer::SPtr mb)
{
    for(auto const& h : hdrs) {
        mb->append((void*)h.key.c_str(), h.key.size());
        mb->append((void*)(char*)": ", 2);
        mb->append((void*)h.value.c_str(), h.value.size());
        mb->append((void*)(char*)"\r\n", 2);
    }
    mb->append((void*)(char*)"\r\n", 2);
}
void fmtMsg(Marvin::MessageBase::SPtr msg_sptr, ContigBuffer::SPtr mb)
{

}
void fmtHeaders4(Marvin::HeaderFields& hdrs, ContigBuffer::SPtr mb)
{
    static const std::string lfcr = "\r\n";
    static const std::string colon{": "};
    for(auto const& h : hdrs) {
        mb->append((void*)h.key.c_str(), h.key.size());
        mb->append((void*)(colon.c_str()), 2);
        mb->append((void*)h.value.c_str(), h.value.size());
        mb->append((void*)(lfcr.c_str()), 2);
    }
    mb->append((void*)(lfcr.c_str()), 2);
}
TEST_CASE("header_serialize_3")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        for(auto const& h : hdrs) {
            int num_bytes = 0;
            memcpy(mb2->nextAvailable(), h.key.c_str(), h.key.size()); mb2->setSize(mb2->size()+h.key.size());
            memcpy(mb2->nextAvailable(), (char*)": ", 2); mb2->setSize(mb2->size() + 2);
            memcpy(mb2->nextAvailable(), h.value.c_str(), h.value.size()); mb2->setSize(mb2->size() + h.value.size());
            assert(num_bytes < 256*4*8);
            memcpy(mb2->nextAvailable(), (char*)"\r\n", 2); mb2->setSize(mb2->size() + 2);
        }
        memcpy(mb2->nextAvailable(), (char*)"\r\n", 2); mb2->setSize(mb2->size() + 2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = fmt_headers_stream(msgSPtr);
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
    CHECK(xy);
    std::cout << "headers serialize 3" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 3" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("header_serialize_4")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        fmtHeaders1(hdrs, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = fmt_headers_stream(msgSPtr);
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
    CHECK(xy);

    std::cout << "headers serialize 4" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 4" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("header_serialize_5")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        fmtHeaders2(hdrs, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = fmt_headers_stream(msgSPtr);
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
    CHECK(xy);
    CHECK((mb2->toString() == fmt_headers_stream(msgSPtr)));
    std::cout << "headers serialize 5" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 5" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("header_serialize_6")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        fmtHeaders3(hdrs, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = fmt_headers_stream(msgSPtr);
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
    CHECK(xy);
        CHECK((mb2->toString() == fmt_headers_stream(msgSPtr)));
    std::cout << "headers serialize 6" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 6" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("header_serialize_7")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        fmtHeaders4(hdrs, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = fmt_headers_stream(msgSPtr);
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
        CHECK(xy);
        CHECK((mb2->toString() == fmt_headers_stream(msgSPtr)));
    std::cout << "headers serialize 7" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 7" << " duration(millisecs): " << duration.count() << std::endl;
}
TEST_CASE("message_serialize_8")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        serialize_headers(*msgSPtr, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
//    std::string ss = fmt_headers_stream(msgSPtr);
//    std::string sss = mb2->toString();
//    bool xy = (ss == sss);
//        CHECK(xy);
//        CHECK((mb2->toString() == fmt_headers_stream(msgSPtr)));
    std::cout << "headers serialize 8" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 8" << " duration(millisecs): " << duration.count() << std::endl;
}
// this give a typical measurement of 17millisecs the previous give 73
TEST_CASE("message_serialize_9")
{
    MessageBase::SPtr msgSPtr = std::make_shared<MessageBase>();
    fillMsgAsResponse_01(msgSPtr);
    auto start = std::chrono::high_resolution_clock::now();
    auto hdrs = msgSPtr->headers();
    ContigBuffer::SPtr mb2;
    for(int i = 0; i < 10000; i++)
    {
        mb2 = makeContigBufferSPtr(256*4*8);
        serialize_headers(*msgSPtr, mb2);
    }
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    auto x = duration.count();
    std::string ss = serialize_headers(*msgSPtr)->toString();
    std::string sss = mb2->toString();
    bool xy = (ss == sss);
        CHECK(xy);
        CHECK((mb2->toString() == serialize_headers(*msgSPtr)->toString()));
    std::cout << "headers serialize 9" << " duration(millisecs): " << duration.count() << std::endl;
    std::cout << "headers serialize 9" << " duration(millisecs): " << duration.count() << std::endl;
}