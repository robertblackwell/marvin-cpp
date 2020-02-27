#include <catch2/catch.hpp>
#include <boost/process.hpp>
#include <marvin/http/marvin_http.hpp>
#include <marvin/helpers/helpers_fs.hpp>
#include "server_runner.hpp"
#include "tp_proxy_runner.hpp"
#include <marvin/forwarding/forward_helpers.hpp>
#include "tp_proxy_tests.hpp"
#include "tp_testcase.hpp"
#include "tp_post.hpp"
#include "tp_tunnel.hpp"
using namespace Marvin;
using namespace Http;
std::vector<tp::TestcaseSPtr> makeWhiteacornTestcases()
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
        Marvin::Uri uri("http://whiteacorn/utests/echo/index.php");
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
        tp::TestcaseSPtr tc = std::make_shared<tp::Testcase>(msg, pScheme, pHost, pPort);
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
#if 1
// sends a request to a know host with predictable response.
// captures the collector output into a file - collector usually writes to a pipe
//      not a file and hence the file must exist and be empty before the start of this test
// and compares to a references file
TEST_CASE("proxy_whiteacorn", "[wa]")
{
    boost::filesystem::path p{__FILE__};
    // because the test definitions are in a different directory
    boost::filesystem::path d = p.parent_path().parent_path() / "test_proxy";
    boost::filesystem::path c = d / "whiteacorn_received";
    boost::filesystem::path e = d / "whiteacorn_expected";
    boost::filesystem::path f = d / "whiteacorn_received_fixed";
    std::string collector_file_path = c.string();

    boost::process::system("/bin/rm", collector_file_path);
    boost::process::system("/usr/bin/touch", collector_file_path);

    std::ofstream outfile(collector_file_path);

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);
    HTTPServer* server_ptr;
    auto proxy_func = [&server_ptr, &outfile](void* param) {
        server_ptr = new HTTPServer([&outfile](boost::asio::io_service& io) {
            CollectorBase* collector = new CollectorBase(io, outfile);
            auto f = new ForwardingHandler(io, collector);
            return f;
        });
        server_ptr->listen(9992);
    };
    std::thread proxy_thread(proxy_func, nullptr);
    server_ptr->terminate();
    proxy_thread.join();
    SECTION("whiteacorn") {
        boost::asio::io_service io;
        auto vect = makeWhiteacornTestcases();
        auto v = vect[0];
        tp::TestcaseSPtr tcSPtr = makeWhiteacornTestcases()[0];
        tp::PostTest post_test(io, tcSPtr);
        post_test.exec();
        io.run();
        sleep(1);
        removeVolatileValues(c, f);
        std::string s_c = Helpers::fs::file_get_contents(c);
        std::string s_f = Helpers::fs::file_get_contents(f);
        std::string s_e = Helpers::fs::file_get_contents(e);
        bool b_e_f = (s_e == s_f);
        bool b_e_c = (s_e == s_c);
        CHECK(s_e == s_f);

    }
    return;
}
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
