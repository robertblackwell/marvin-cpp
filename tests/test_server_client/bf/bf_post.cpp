#include <catch2/catch.hpp>
#include "json.hpp"
#include "forward_helpers.hpp"
#include "server_connection_manager.hpp"
#include "bf_post.hpp"

using namespace Marvin;
using namespace Marvin::Http;

using namespace body_format;
using json = nlohmann::json;

PostTest::PostTest(boost::asio::io_service& io, Testcase testcase): m_io(io), m_testcase(testcase)
{

}
void PostTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    Marvin::BufferChainSPtr bsp = rdr->getContentBuffer();
    std::string raw_body = bsp->to_string();
    json j = json::parse(raw_body);
    std::string sx = j["req"]["body"];
    
    std::string sy = m_testcase.buffers_as_string();
#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(sx == sy);
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(sx == sy);
    
//    std::cout << "SUCCESS: " << _testcase._description <<  std::endl;
    if(rdr->getHeader(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionClose) {
        m_client->close();
        m_client = nullptr;
    }
    
//    std::cout << "PostTest::" << _testcase._description << std::endl;
}

void PostTest::exec()
{
    auto url = m_testcase.m_url;
    /// \todo - this is the place to fill the message so that client does not have
    /// to do it.
    m_uri_sptr = std::make_shared<Marvin::Uri>(url);
    m_client = std::shared_ptr<Client>(new Client(m_io, *m_uri_sptr));
    m_msg = std::shared_ptr<MessageBase>(new MessageBase());
    m_msg->setMethod(HttpMethod::POST);
    helpers::applyUriNonProxy(m_msg, *m_uri_sptr);
    m_msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);

    auto f = std::bind(&PostTest::handler, this, std::placeholders::_1, std::placeholders::_2);

    switch(m_testcase.m_test_type){
        case TestType::WITH_STRING :
        {
            auto str_buf = m_testcase.buffers_as_string();
            m_msg->setContent(m_testcase.buffers_as_string());
            m_client->asyncWrite(m_msg, str_buf, f);
            break;
        }
            
        case TestType::WITH_MBUFFER :
        {
            auto mbufptr = m_testcase.buffers_as_mbuffer();
            m_msg->setContent(Marvin::BufferChain::makeSPtr(m_testcase.buffers_as_mbuffer()));
            m_client->asyncWrite(m_msg, mbufptr, f);
            break;
        }
            
        case TestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = m_testcase.buffers_as_buffer_chain();
            m_msg->setContent(m_testcase.buffers_as_buffer_chain() );
            m_client->asyncWrite(m_msg, tmp, f);
            break;
        }
    }

}

