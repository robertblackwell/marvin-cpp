#include <catch/catch.hpp>
#include "tp_helpers.hpp"
#include "tp_proxy_tests.hpp"
#include "tp_testcase.hpp"
#include "tp_post.hpp"

std::vector<tp::TestcaseSPtr> makeTestcases()
{
    std::string pScheme = "http";
    std::string pHost = "localhost";
    std::string pPort = "9992";
    
    std::vector<tp::TestcaseSPtr> msgTable;
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy mat turn that into a relative url
        msg->setUri("/echo");
        msg->setHeader(HttpHeader::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(HttpHeader::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(HttpHeader::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(HttpHeader::Name::TransferEncoding,"chunked");
        msg->setHeader(HttpHeader::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        setContent(*msg, bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    return msgTable;
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy mat turn that into a relative url
        msg->setUri("http://localhost:9991/somepath/script.php?parm=123456#fragment");
        msg->setHeader(HttpHeader::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(HttpHeader::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(HttpHeader::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(HttpHeader::Name::TransferEncoding,"chunked");
        msg->setHeader(HttpHeader::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        setContent(*msg, bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy mat turn that into a relative url
        msg->setUri("http://localhost:9991/somepath/script.php?parm=123456#fragment");
        msg->setHeader(HttpHeader::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(HttpHeader::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(HttpHeader::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
        msg->setHeader(HttpHeader::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        setContent(*msg, bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    
    return msgTable;
}
TEST_CASE("mtim", "first")
{
    return;
    boost::asio::io_service io;
    auto vect = makeTestcases();
    auto v = vect[0];
    tp::TestcaseSPtr tcSPtr = makeTestcases()[0];
    tp::PostTest r(io, tcSPtr);
    r.exec();
    io.run();
}
