#include <catch2/catch.hpp>
#include <boost/process.hpp>
#include "marvin_http.hpp"
#include "helpers_fs.hpp"
#include "collector_base.hpp"
#include "server_runner.hpp"
#include "tp_proxy_runner.hpp"
#include "forward_helpers.hpp"
#include "tp_proxy_tests.hpp"
#include "tp_testcase.hpp"
#include "tp_post.hpp"
#include "tp_tunnel.hpp"
#include "proxy_fixture.hpp"

using namespace Marvin;
using namespace Http;

std::vector<tp::TestcaseSPtr> makeWhiteacornTestcases(
        std::string uriString,
        std::string proxyScheme,
        std::string proxyHost,
        std::string proxyPort
)
{
    /// this sends the request to our mitm proxy
    std::string pScheme = "http";
    std::string pHost = "localhost";
    std::string pPort = "9992";
    
    std::vector<tp::TestcaseSPtr> msgTable;
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy may turn that into a relative url
//        Marvin::Uri uri("http://whiteacorn/utests/echo/index.php");
        Marvin::Uri uri(uriString);
        helpers::applyUriProxy(msg, uri);
//        msg->setUri("http://localhost/echo");
//        msg->setHeader(Marvin::Http::Headers::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(Marvin::Http::Headers::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(Marvin::Http::Headers::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(Marvin::Http::Headers::Name::TransferEncoding,"chunked");
        msg->setHeader(Marvin::Http::Headers::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        msg->setContent(bdy);

        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, proxyScheme, proxyHost, proxyPort);
        msgTable.push_back(tc);
    }
    return msgTable;
}
std::vector<tp::TestcaseSPtr> makeTestServerTestcases()
{
    /// this sends the request to our mitm proxy
    std::string pScheme = "http";
    std::string pHost = "localhost";
    std::string pPort = "9992";
#if 1
    std::vector<tp::TestcaseSPtr> msgTable;
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy mat turn that into a relative url
//        msg->setUri("http://localhost:9991/somepath/script.php?parm=123456#fragment");
        msg->setUri("http://localhost:9991/echo");
        msg->setHeader(Marvin::Http::Headers::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(Marvin::Http::Headers::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(Marvin::Http::Headers::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(Marvin::Http::Headers::Name::TransferEncoding,"chunked");
        msg->setHeader(Marvin::Http::Headers::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        msg->setContent(bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
        msg->setMethod(HTTP_POST);
        // note requests through a proxy must provide absolute uri on the first line
        // proxy mat turn that into a relative url
        msg->setUri("http://localhost:9991/somepath/script.php?parm=123456#fragment");
        msg->setHeader(Marvin::Http::Headers::Name::Host, "localhost:9991");
        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(Marvin::Http::Headers::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(Marvin::Http::Headers::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
        msg->setHeader(Marvin::Http::Headers::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "012345678956";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        msg->setContent(bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    
    return msgTable;
#endif
}
std::vector<tp::TestcaseSPtr> makeConnectRequestTestcases()
{
    /// this sends the request to our mitm proxy
    std::string pScheme = "http";
    std::string pHost = "localhost";
    std::string pPort = "9992";
#if 1
    std::vector<tp::TestcaseSPtr> msgTable;
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
         Marvin::Uri uri("http://whiteacorn/utests/echo/index.php");
        Marvin::Http::makeProxyRequest(*msg, HttpMethod::POST, uri);

        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(Marvin::Http::Headers::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(Marvin::Http::Headers::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(Marvin::Http::Headers::Name::TransferEncoding,"chunked");
        msg->setHeader(Marvin::Http::Headers::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        msg->setContent(bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    {
        MessageBaseSPtr msg = std::make_shared<MessageBase>();
         Marvin::Uri uri("https://ssltest:443/echo");
        Marvin::Http::makeProxyRequest(*msg, HttpMethod::GET, uri);

        msg->setHeader("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
        msg->setHeader(
            "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
        msg->setHeader("Accept-Language","en");
        msg->setHeader("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
        msg->setHeader(Marvin::Http::Headers::Name::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");
        msg->setHeader(Marvin::Http::Headers::Name::Connection,"Keep-Alive, TE");
        msg->setHeader("TE","deflate, gzip, chunked, trailer");
//        msg->setHeader(Marvin::Http::Headers::Name::TransferEncoding,"chunked");
        msg->setHeader(Marvin::Http::Headers::Name::ETag,"1928273tefadseercnbdh");
        std::string s = "";
        Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
        msg->setContent(bdy);
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
        msgTable.push_back(tc);
    }
    return msgTable;
#endif
}

void removeVolatileValues(boost::filesystem::path inFile, boost::filesystem::path outFile) {
    std::system( (std::string("/usr/bin/sed -e '/^DATE/d' -e '/junk/d' -e '/body/d' ") + inFile.string() + " > " + outFile.string()).c_str() );
}
// sends a request to a know host with predictable response.
// captures the collector output into a file - collector usually writes to a pipe
//      not a file and hence the file must exist and be empty before the start of this test
// and compares to a references file
TEST_CASE_METHOD(ProxyFixture, "whiteacorn_post")
{
#if 1
    boost::asio::io_service io;
    // get a testcase
    auto testcases = makeWhiteacornTestcases(
            "http://whiteacorn/utests/echo/index.php",
            this->m_proxy_scheme,
            this->m_proxy_host,
            this->m_proxy_port
            );
    tp::TestcaseSPtr tcSPtr = testcases[0];
    // use the testcase to issue a post request
    tp::PostTest post_test(io, tcSPtr);
    post_test.exec();
    io.run();
    // wait for the dust to settle
    sleep(1);
    // now test that we got the expected result
    removeVolatileValues(m_received, m_received_fixed);
    std::string s_c = Helpers::fs::file_get_contents(m_received);
    std::string s_f = Helpers::fs::file_get_contents(m_received_fixed);
    std::string s_e = Helpers::fs::file_get_contents(this->expectedFilePathForTest("whiteacorn"));
    bool b_e_f = (s_e == s_f);
    bool b_e_c = (s_e == s_c);
    CHECK(s_e == s_f);
}
#endif
#if 1
TEST_CASE_METHOD(ProxyFixture, "curl_get")
{
    boost::process::system("/usr/bin/curl", "-x", "localhost:9992", "http://whiteacorn.com");
    std::string s_c = Helpers::fs::file_get_contents(m_received);
    std::string s_f = Helpers::fs::file_get_contents(m_received_fixed);
    std::string s_e = Helpers::fs::file_get_contents(this->expectedFilePathForTest("curl_get"));
    bool b_e_f = (s_e == s_f);
    bool b_e_c = (s_e == s_c);}
#endif

#if 0
TEST_CASE("proxy_testserver", "[ts]")
{
    boost::asio::io_service io;
    tp::TestcaseSPtr tcSPtr = makeTestServerTestcases()[0];
    tp::PostTest post_test(io, tcSPtr);
    post_test.exec();
    io.run();
//    stopProxyServer();
//    stopTestServer();
}
#endif
#if 0
TEST_CASE("proxy_tunnel_whiteacorn/utests/echo/index.php:80", "[ts]")
{
    boost::asio::io_service io;
    tp::TestcaseSPtr tcSPtr = makeConnectRequestTestcases()[0];
    tp::Tunnel tunnel(io, tcSPtr);
    tunnel.exec();
    io.run();
}
#endif
#if 0
TEST_CASE("proxy_tunnel_ssltest/echo:443", "[ts]")
{
    boost::asio::io_service io;
    tp::TestcaseSPtr tcSPtr = makeConnectRequestTestcases()[1];
    tp::Tunnel tunnel(io, tcSPtr);
    tunnel.exec();
    io.run();
}
#endif
#if 0
TEST_CASE("proxy_connect_ssltest", "[ts]")
{
    boost::asio::io_service io;
    tp::TestcaseSPtr tcSPtr = makeConnectRequestTestcases()[1];
    tp::PostTest post(io, tcSPtr);
    post.exec();
    io.run();
}
#endif
