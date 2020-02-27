#include <catch2/catch.hpp>
#include <marvin/external_src/json.hpp>
#include "tp_pipeline.hpp"

using namespace tp;
using json = nlohmann::json;

PipelineTest::PipelineTest(boost::asio::io_service& io, std::vector<Testcase> testcase): _io(io), _testcase(testcase)
{
    _msg_index = 0;
//    std::string url = _testcase[0]._url;
    _client = std::shared_ptr<Client>(new Client(_io, url));
}
void PipelineTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    std::string raw_body = rdr->getBody()->to_string();
    json j = json::parse(raw_body);
    std::string req_body = j["req"]["body"];
    std::string ch_uuid = j["xtra"]["connection_handler_uuid"];
    std::string rh_uuid = j["xtra"]["request_handler_uuid"];
    std::string srwbdy = _testcase[_msg_index].buffers_as_string();

    /// test server is handling connection keep-alive and close correctly
    /// all but last response should be keep-alive
    /// last one should be close
    /// this echos what we sent
    std::string connection_hdr = rdr->getHeader(Marvin::Http::Headers::Name::Connection);
    if(_msg_index < (_testcase.size() - 1)) {
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
    if( _msg_index == 0 ) {
        _ch_uuid = ch_uuid;
        _rh_uuid = rh_uuid;
    } else {
        assert(ch_uuid == _ch_uuid);
        assert(rh_uuid == _rh_uuid);
        REQUIRE(ch_uuid == _ch_uuid);
        REQUIRE(rh_uuid == _rh_uuid);
    }
    
    std::string sx = rdr->getBody()->to_string();
    std::string expected_req_body = _testcase[_msg_index].buffers_as_string();

#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(req_body == expected_req_body);
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(req_body == expected_req_body);

//    std::cout << "Pipeline::" << _testcase[_msg_index]._description <<  std::endl;
    _msg_index++;
    if (_msg_index < _testcase.size()) {
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
    const Testcase& tc = _testcase[_msg_index];
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    _msg->setMethod(HttpMethod::POST);
    
    /// all but the last message are keep-alive
    if( _msg_index < (_testcase.size() - 1))
        _msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionKeepAlive);
    else
        _msg->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);

    auto f = std::bind(&PipelineTest::handler, this, std::placeholders::_1, std::placeholders::_2);

    switch(tc._test_type){
        case TestType::WITH_STRING :
        {
            auto str_buf = tc.buffers_as_string();
            _client->asyncWrite(_msg, str_buf, f);
            break;
        }
            
        case TestType::WITH_MBUFFER :
        {
            auto mbufptr = tc.buffers_as_mbuffer();
            _client->asyncWrite(_msg, mbufptr, f);
            break;
        }
            
        case TestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = tc.buffers_as_buffer_chain();
            _client->asyncWrite(_msg, tmp, f);
            break;
        }
    }

}

