#include <doctest/doctest.h>

#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>
#include <marvin/http/message_factory.hpp>
#include <marvin/helpers/helpers_fs.hpp>
#include <marvin/collector/collector_base.hpp>
#include <marvin/forwarding/forward_helpers.hpp>
#include "test_headers.hpp"
#include "tp_testcase.hpp"
#include "proxy_fixture.hpp"

using namespace Marvin;
using namespace Http;
using json = nlohmann::json;

namespace  {

    struct kv_t {
        std::string k;
        std::string v;
    };

    std::map<std::string, std::string> xjsonParseHeaders(json jsonHeaders) {
        typedef std::vector<kv_t> header_list;
        header_list hl{};
        std::map<std::string, std::string> hm{};
        for (json::iterator it = jsonHeaders.begin(); it != jsonHeaders.end(); ++it) {
    //        std::cout << it.key() << " : " << it.value() << "\n";
            auto z = *it;
            std::string k = it.key();
            if (it.value().is_string()) {
                std::string v = it.value();
                kv_t akv = kv_t{.k = k, .v = v};
                hm.insert({k, v});
                hl.push_back(akv);
            } else if (it.value().is_number_integer()) {
                int v = it.value().get<int>();
                std::string vstr = std::to_string(v);
                kv_t akv = kv_t{.k = k, .v = vstr};
                hm.insert({k, vstr});
                hl.push_back(akv);
            } else if (it.value().is_number_float()) {
                double v = it.value().get<float>();
                std::string vstr = std::to_string(v);
                kv_t akv = kv_t{.k = k, .v = vstr};
                hm.insert({k, vstr});
                hl.push_back(akv);
            } else {
                throw "invalid type of value in header list";
            }
    //      hm.insert({k, v});
        }
        return hm;
    }

    /**
     * This class sends the testcase message gets, and analyses, the response
    */
    class ProxyRequestor
    {
    public:
        ProxyRequestor( boost::asio::io_service& io,
                tp::TestcaseSPtr testcaseSPtr,
                std::string proxy_scheme,
                std::string proxy_host,
                std::string proxy_port
                );
        void handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
        void exec();
    protected:
        boost::asio::io_service&        m_io;
        Marvin::Http::MessageBaseSPtr   m_msg_sptr;
        ClientSPtr                      m_client_sptr;
        tp::TestcaseSPtr                    m_testcase_sptr;
        std::string                     m_scheme;
        std::string                     m_proxy_host;
        std::string                     m_proxy_port;
    };

    ProxyRequestor::ProxyRequestor(boost::asio::io_service& io,
            tp::TestcaseSPtr testcaseSPtr,
            std::string proxy_scheme,
            std::string proxy_host,
            std::string proxy_port
    ): m_io(io), m_testcase_sptr(testcaseSPtr)
    {
        m_msg_sptr = m_testcase_sptr->m_msg_sptr;
        m_scheme = proxy_scheme;
        m_proxy_host = proxy_host;
        m_proxy_port = proxy_port;
    }
    void ProxyRequestor::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        Marvin::BufferChainSPtr bsp = rdr->getContentBuffer();
        std::string raw_body = bsp->to_string();
    //    std::cout << raw_body << std::endl;
        /// check correct status
        CHECK(rdr->statusCode() == 200);

        /// now test echo correctly
        json j;
        /// the content of the respons should be json text
        try{
            j = json::parse(raw_body);
        } catch(std::exception& e) {
            std::cout << "json error " << e.what() << std::endl;
            INFO("json parse failed");
            REQUIRE(false);
            return;
        }
        auto ra = j["req"];
        auto hh = ra["headers"];
        std::string l32 = hh["CONTENT-LENGTH"];
        std::string s33 = Marvin::Http::Headers::Name::ContentLength;
        std::string l2 = hh[Marvin::Http::Headers::Name::ContentLength];
        std::string ll = ra["headers"]["CONTENT-LENGTH"];
        struct kv_t {
            std::string k;
            std::string v;
        };
        typedef std::vector<kv_t> header_list;
        header_list hl{};
        std::map<std::string, std::string> hm{};
        std::map<std::string, std::string> echoedHeaders = xjsonParseHeaders(j["req"]["headers"]);

        ///
        /// ///////////////////////////////////////////////////////////////////////////////////
        /// @warn - if any of the stuff below throws an exception first check that the server
        /// has sent back a json string that is correctly formated - the most likely problem
        /// is that the json_encode of the headers has not been done correctly
        /// ///////////////////////////////////////////////////////////////////////////////////
        ///
        /// check method
        auto echoed_method = j["req"]["method"].get<std::string>();
        auto original_method = m_testcase_sptr->m_msg_sptr->getMethodAsString();
        CHECK( (echoed_method == original_method) );
        /// check headers
    //    auto reqq = j["req"];
    //    std::string ech_body = j["req"]["body"];
    //    size_t len = ech_body.size();
    //    auto jj = j["req"]["headers"];
    //    auto echoed_headers2 = (j["req"]["headers"]);
        auto echoed_headers = test::helpers::headersFromJson(j["req"]["headers"]);
    //    std::string lenstr = echoed_headers[Marvin::Http::Headers::Name::ContentLength];

        auto original_headers = m_testcase_sptr->m_msg_sptr->getHeaders();
        /// these are the headers that should be preserved - they arer [present in every test case just for convenience
        CHECK( (echoed_headers["ACCEPT"] == original_headers["ACCEPT"]) );
        CHECK( (echoed_headers["ACCEPT-CHARSET"] == original_headers["ACCEPT-CHARSET"]) );
        CHECK( (echoed_headers["ACCEPT-LANGUAGE"] == original_headers["ACCEPT-LANGUAGE"]) );
        CHECK( (echoed_headers[Marvin::Http::Headers::Name::ContentLength] == original_headers[Marvin::Http::Headers::Name::ContentLength]) );
        CHECK( (echoed_headers[Marvin::Http::Headers::Name::Host] == original_headers[Marvin::Http::Headers::Name::Host]) );
        CHECK( (echoed_headers["USER-AGENT"] == original_headers["USER-AGENT"]) );
        /// these headers should be present but changed
        CHECK(echoed_headers.has(Marvin::Http::Headers::Name::Connection));
        /// the proxy only allows connection close
        std::string sc = boost::to_upper_copy(echoed_headers.get(Marvin::Http::Headers::Name::Connection));
        CHECK(boost::to_upper_copy(echoed_headers.get(Marvin::Http::Headers::Name::Connection)) == Marvin::Http::Headers::Value::ConnectionClose);

        CHECK(echoed_headers.has(Marvin::Http::Headers::Name::AcceptEncoding));

        /// check body
        std::string echoedBody = j["req"]["body"];
        std::string originalBody = (m_testcase_sptr->m_msg_sptr->getContentBuffer())->to_string();
        CHECK(echoedBody == originalBody);

        if(rdr->getHeader(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionClose) {
            m_client_sptr->close();
            m_client_sptr = nullptr;
        }

    //    std::cout << "PostTest::" << _testcase._description << std::endl;
    }
    void ProxyRequestor::exec()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        m_client_sptr = std::shared_ptr<Client>(new Client(m_io, m_scheme, m_proxy_host, m_proxy_port));

        auto f = std::bind(&ProxyRequestor::handler, this, std::placeholders::_1, std::placeholders::_2);

        /// generate a random string and use it as the message content
    #if 0
        boost::uuids::uuid tmp_uuid = boost::uuids::random_generator()();
    std::string tmps = boost::uuids::to_string(tmp_uuid);
    m_msg_sptr->setContent(tmps);
    m_testcase_sptr->m_msg_sptr->setContent(tmps);
    #endif
        auto buf = m_testcase_sptr->m_msg_sptr->getContentBuffer();
        m_client_sptr->asyncWrite(m_msg_sptr, buf, f);

    }


    tp::TestcaseSPtr makePostRequestTestcase(
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

        tp::TestcaseSPtr tc;
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

            tc = std::make_shared<tp::Testcase>(msg, proxyScheme, proxyHost, proxyPort);
        }
        return tc;
    }

    void removeVolatileValues(boost::filesystem::path inFile, boost::filesystem::path outFile) {
        auto retcode = std::system( (std::string("sed -e '/X-FORWARDED-FOR/d' -e '/^DATE/d' -e '/junk/d' -e '/body/d' ") + inFile.string() + " > " + outFile.string()).c_str() );
        REQUIRE_MESSAGE(retcode == 0, "SED command failed removing volatile data from whiteacorn.com received");
    }


} // namespace

// sends a request to a know host with predictable response.
// captures the collector output into a file - collector usually writes to a pipe
//      not a file and hence the file must exist and be empty before the start of this test
// and compares to a references file

TEST_CASE_FIXTURE(ProxyFixture, "whiteacorn_post")
{
    boost::asio::io_service io;
    // get a testcase
    tp::TestcaseSPtr  tcSPtr = makePostRequestTestcase(
            std::string("http://whiteacorn.com/utests/echo/index.php"),
            this->m_proxy_scheme,
            this->m_proxy_host,
            std::to_string(this->m_proxy_port)
            );
    // use the testcase to issue a post request
    ProxyRequestor post_test(
        io,
            tcSPtr,
            this->m_proxy_scheme,
            this->m_proxy_host,
            std::to_string(this->m_proxy_port)
    );
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
