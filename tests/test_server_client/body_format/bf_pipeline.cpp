#include "catch.hpp"
#include "bf_pipeline.hpp"

using namespace body_format;

PipelineTest::PipelineTest(boost::asio::io_service& io, std::vector<Testcase>& testcase): _io(io), _testcase(testcase)
{
    _msg_index = 0;
    std::string url = _testcase[0]._url;
    _client = std::shared_ptr<Client>(new Client(_io, url));
}
void PipelineTest::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    /// test server is handling connection keep-alive and close correctly
    /// all but last response should be keep-alive
    /// last one should be close
    /// this echos what we sent
    std::string connection_hdr = rdr->getHeader(HttpHeader::Name::Connection);
    if(_msg_index < (_testcase.size() - 1)) {
        assert(connection_hdr == HttpHeader::Value::ConnectionKeepAlive);
        REQUIRE(connection_hdr == HttpHeader::Value::ConnectionKeepAlive);
    } else {
        assert(connection_hdr == HttpHeader::Value::ConnectionClose);
        REQUIRE(connection_hdr == HttpHeader::Value::ConnectionClose);
    }
    /**
    * Test that the server is using the same connection handler and request handler for each request
    * On first response save uuids and then test subsequence uuids against that saved value
    * instances of connection handler and request handler have their own uuid.
    * Request handler puts these in the response header
    */
    std::string ch_uuid = rdr->getHeader(HttpHeader::Name::ConnectionHandlerId);
    std::string rh_uuid = rdr->getHeader(HttpHeader::Name::RequestHandlerId);
    if( _msg_index == 0 ) {
        _ch_uuid = ch_uuid;
        _rh_uuid = rh_uuid;
    } else {
        assert(ch_uuid == _ch_uuid);
        assert(rh_uuid == _rh_uuid);
        REQUIRE(ch_uuid == _ch_uuid);
        REQUIRE(rh_uuid == _rh_uuid);
    }
    
    std::string sx = rdr->get_body_chain().to_string();
    std::string sy = _testcase[_msg_index].buffers_as_string();

#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(sx == sy);
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(sx == sy);

    std::cout << "Pipeline::" << _testcase[_msg_index]._description <<  std::endl;
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
    Testcase& tc = _testcase[_msg_index];
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    _msg->setMethod(HttpMethod::POST);
    
    /// all but the last message are keep-alive
    if( _msg_index < (_testcase.size() - 1))
        _msg->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionKeepAlive);
    else
        _msg->setHeader(HttpHeader::Name::Connection, HttpHeader::Value::ConnectionClose);

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

