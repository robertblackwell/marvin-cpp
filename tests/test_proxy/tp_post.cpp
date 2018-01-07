#include <catch/catch.hpp>
#include "json.hpp"
#include "test_headers.hpp"
#include "server_connection_manager.hpp"
#include "tp_post.hpp"

using namespace tp;
using json = nlohmann::json;
namespace tp{
PostTest::PostTest(boost::asio::io_service& io, tp::TestcaseSPtr testcaseSPtr): m_io(io), m_testcase_sptr(testcaseSPtr)
{
    m_msg_sptr = m_testcase_sptr->m_msg_sptr;
    m_scheme = m_testcase_sptr->m_proxy_scheme;
    m_proxy_host = m_testcase_sptr->m_proxy_host;
    m_proxy_port = m_testcase_sptr->m_proxy_port;
}
void PostTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    Marvin::BufferChainSPtr bsp = rdr->getContentBuffer();
    std::string raw_body = bsp->to_string();
//    std::cout << raw_body << std::endl;
    json j;
    /// the content of the respons should be json text
    try{
        j = json::parse(raw_body);
    } catch(std::exception& e) {
        std::cout << "json error " << e.what() << std::endl;
        INFO("json parse failed");
        CHECK(false);
        return;
    }
    auto ra = j["req"];
    auto echoed_headers = test::helpers::headersFromJson(j["req"]["headers"]);
    auto original_headers = m_testcase_sptr->m_msg_sptr->getHeaders();
    /// cannot test header equality as the mitm proxy has changed the headers
//    auto b = test::helpers::checkHeaders(echoed_headers, original_headers);
//    CHECK(b);
    std::string echoedBody = j["req"]["body"];
    
    std::string originalBody = (m_testcase_sptr->m_msg_sptr->getContentBuffer())->to_string();
    
#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
    assert(rdr->statusCode() == 200);
    assert(echoedBody == originalBody);
#endif
    CHECK(rdr->statusCode() == 200);
    CHECK(echoedBody == originalBody);
    if(rdr->getHeader(HttpHeader::Name::Connection) == HttpHeader::Value::ConnectionClose) {
        m_client_sptr->close();
        m_client_sptr = nullptr;
    }
    
//    std::cout << "PostTest::" << _testcase._description << std::endl;
}

void PostTest::exec()
{
    
    m_client_sptr = std::shared_ptr<Client>(new Client(m_io, m_scheme, m_proxy_host, m_proxy_port));

    auto f = std::bind(&PostTest::handler, this, std::placeholders::_1, std::placeholders::_2);

    auto buf = m_testcase_sptr->m_msg_sptr->getContentBuffer();
    m_client_sptr->asyncWrite(m_msg_sptr, buf, f);

}

} // namespace
