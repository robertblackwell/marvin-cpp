#include "catch.hpp"
#include "tsc_pipeline.hpp"

PipelineTest::PipelineTest(boost::asio::io_service& io, std::vector<EchoTestcase>& testcase): _io(io), _testcase(testcase)
{
    _msg_index = 0;
    std::string url = _testcase[0]._url;
    _client = std::shared_ptr<Client>(new Client(_io, url));
}
void PipelineTest::handler(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)
{
    /**
    * Test that the server is using the same connection for each request
    * on first response save uuid and then test subsequence uuids against that saved value
    * server provides the uuid to the request handler. Request handler puts in in the response
    * header
    */
    std::string uuid = rdr->getHeader(HttpHeader::Name::ConnectionHandlerId);
    if( _msg_index == 0 ) {
        _uuid = uuid;
    } else {
        assert(uuid == _uuid);
        REQUIRE(uuid == _uuid);
    }
    
    std::string sx = rdr->get_body_chain().to_string();
    std::string sy = _testcase[_msg_index].buffers_as_string();

#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
    assert(rdr->statusCode() == 200);
    assert(sx == sy);
#endif
    REQUIRE(rdr->statusCode() == 200);
    REQUIRE(sx == sy);

    _msg_index++;
    if (_msg_index < _testcase.size()) {
        exec();
    } else {
//        std::cout << "SUCCESS: " << _testcase[_msg_index]._description <<  std::endl;
    }
    
}

/**
* This starts the write of a series of messages
*/
void PipelineTest::exec()
{
    EchoTestcase& tc = _testcase[_msg_index];
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    _msg->setMethod(HttpMethod::POST);

    auto f = std::bind(&PipelineTest::handler, this, std::placeholders::_1, std::placeholders::_2);

    switch(tc._test_type){
        case EchoTestType::WITH_STRING :
        {
            auto str_buf = tc.buffers_as_string();
            _client->asyncWrite(_msg, str_buf, f);
            break;
        }
            
        case EchoTestType::WITH_MBUFFER :
        {
            auto mbufptr = tc.buffers_as_mbuffer();
            _client->asyncWrite(_msg, mbufptr, f);
            break;
        }
            
        case EchoTestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = tc.buffers_as_buffer_chain();
            _client->asyncWrite(_msg, tmp, f);
            break;
        }
    }

}

