#include <doctest/doctest.h>
#include <marvin/forwarding/forward_helpers.hpp>
#include <json/json.hpp>
#include "bf_pipeline.hpp"

using namespace Marvin;
using namespace Marvin::Http;

using namespace body_format;
using json = nlohmann::json;

PipelineTest::PipelineTest(boost::asio::io_service& io, std::vector<Testcase> testcase): m_io(io), m_testcase(testcase)
{
    m_msg_index = 0;
    std::string url = m_testcase[0].m_url;
    m_uri_sptr = std::make_shared<Marvin::Uri>(url);
    /// \todo - this is the place to fill the message so that client does not have
    /// to do it.
    m_client = std::shared_ptr<Client>(new Client(m_io, *m_uri_sptr));
}
void PipelineTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    std::string raw_body = rdr->getContentBuffer()->to_string();
    json j = json::parse(raw_body);
    std::string req_body = j["req"]["body"];
    std::string ch_uuid = j["xtra"]["connection_handler_uuid"];
    std::string rh_uuid = j["xtra"]["request_handler_uuid"];
    std::string srwbdy = m_testcase[m_msg_index].buffers_as_string();

    /// test server is handling connection keep-alive and close correctly
    /// all but last response should be keep-alive
    /// last one should be close
    /// this echos what we sent
    std::string connection_hdr = rdr->getHeader(Marvin::Http::Headers::Name::Connection);
    if(m_msg_index < (m_testcase.size() - 1)) {
        assert(connection_hdr == Marvin::Http::Headers::Value::ConnectionKeepAlive);
        REQUIRE(connection_hdr == Marvin::Http::Headers::Value::ConnectionKeepAlive);
    } else {
        assert(connection_hdr == Marvin::Http::Headers::Value::ConnectionClose);
        REQUIRE(connection_hdr == Marvin::Http::Headers::Value::ConnectionClose);
    }
    /**
    * Test that the server is using the same connection handler and request handler for each request
    * On first response save uuids and then test subsequence uuids against that saved value
    * instances of connection handler and request handler have their own uuid.
    * Request handler puts these in the response header
    */
//    std::string ch_uuid = rdr->getHeader(Marvin::Http::Headers::Name::ConnectionHandlerId);
//    std::string rh_uuid = rdr->getHeader(Marvin::Http::Headers::Name::RequestHandlerId);
    if( m_msg_index == 0 ) {
        m_ch_uuid = ch_uuid;
        m_rh_uuid = rh_uuid;
    } else {
        assert(ch_uuid == m_ch_uuid);
        assert(rh_uuid == m_rh_uuid);
        REQUIRE(ch_uuid == m_ch_uuid);
        REQUIRE(rh_uuid == m_rh_uuid);
    }
    
    std::string sx = rdr->getContentBuffer()->to_string();
    std::string expected_req_body = m_testcase[m_msg_index].buffers_as_string();

#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(req_body == expected_req_body);
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(req_body == expected_req_body);

//    std::cout << "Pipeline::" << _testcase[_msg_index]._description <<  std::endl;
    m_msg_index++;
    if (m_msg_index < m_testcase.size()) {
        exec();
    } else {
//        std::cout << "Pipeline::" << _testcase[_msg_index]._description <<  std::endl;
    }

}

/**
* This starts the write of a series of messages
*/
void PipelineTest::exec()
{
    const Testcase& tc = m_testcase[m_msg_index];
    m_msg = std::shared_ptr<MessageBase>(new MessageBase());
    m_msg->setMethod(HttpMethod::POST);
    helpers::applyUriNonProxy(m_msg, *m_uri_sptr);
    /// all but the last message are keep-alive
    if( m_msg_index < (m_testcase.size() - 1))
        m_msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionKeepAlive);
    else
        m_msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);

    auto f = std::bind(&PipelineTest::handler, this, std::placeholders::_1, std::placeholders::_2);
    switch(tc.m_test_type){
        case TestType::WITH_STRING :
        {
            auto str_buf = tc.buffers_as_string();
            m_msg->setContent(tc.buffers_as_string());
            m_client->asyncWrite(m_msg, str_buf, f);
            break;
        }
            
        case TestType::WITH_MBUFFER :
        {
            auto mbufptr = tc.buffers_as_mbuffer();
            m_msg->setContent(Marvin::BufferChain::makeSPtr(tc.buffers_as_mbuffer()));
            m_client->asyncWrite(m_msg, mbufptr, f);
            break;
        }
            
        case TestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = tc.buffers_as_buffer_chain();
            m_msg->setContent(tc.buffers_as_buffer_chain() );
            m_client->asyncWrite(m_msg, tmp, f);
            break;
        }
    }

}

