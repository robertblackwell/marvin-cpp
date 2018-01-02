#include <catch/catch.hpp>
#include "json.hpp"
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
    json j = json::parse(raw_body);
    std::string sx = j["req"]["body"];
    
    std::string sy = (m_testcase_sptr->m_msg_sptr->getContentBuffer())->to_string();
#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(sx == sy);
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(sx == sy);
    
//    std::cout << "SUCCESS: " << _testcase._description <<  std::endl;
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
