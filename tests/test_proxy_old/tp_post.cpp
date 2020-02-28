#include <catch2/catch.hpp>
#include <json/json.hpp>
#include "test_headers.hpp"
#include <marvin/server/server_connection_manager.hpp>
#include "tp_post.hpp"

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

using namespace tp;
using json = nlohmann::json;
namespace tp {
PostTest::PostTest(boost::asio::io_service& io, tp::TestcaseSPtr testcaseSPtr): m_io(io), m_testcase_sptr(testcaseSPtr)
{
    m_msg_sptr = m_testcase_sptr->m_msg_sptr;
    m_scheme = m_testcase_sptr->m_proxy_scheme;
    m_proxy_host = m_testcase_sptr->m_proxy_host;
    m_proxy_port = m_testcase_sptr->m_proxy_port;
}
void PostTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
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
    CHECK(echoed_headers.get(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionClose);

    CHECK(echoed_headers.has(Marvin::Http::Headers::Name::AcceptEncoding));
    CHECK(echoed_headers.has(Marvin::Http::Headers::Name::TE));

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

void PostTest::exec()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    m_client_sptr = std::shared_ptr<Client>(new Client(m_io, m_scheme, m_proxy_host, m_proxy_port));

    auto f = std::bind(&PostTest::handler, this, std::placeholders::_1, std::placeholders::_2);

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

} // namespace
