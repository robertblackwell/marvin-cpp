#include <gtest/gtest.h>
#include "tsc_post.hpp"


/**
* This fuunction runs a post request based on a testcase and checks the response is as expected
*/
std::shared_ptr<Client> post_body_testcase(EchoTestcase& testcase, boost::asio::io_service& io)
{
    auto url = testcase._url;
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, url));

    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    msg->setMethod(HttpMethod::POST);

    std::function<void(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)>
    f = [client, msg, &testcase](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr)
    {
        std::string sx = rdr->get_body_chain().to_string();
        std::string sy = testcase.buffers_as_string();
#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
        assert(rdr->statusCode() == 200);
        assert(sx == sy);
        std::cout << "SUCCESS: " << testcase._description <<  std::endl;
        
    };
    switch(testcase._test_type){
        case EchoTestType::WITH_STRING :
        {
            auto str_buf = testcase.buffers_as_string();
            client->asyncWrite(msg, str_buf, f);
            break;
        }
            
        case EchoTestType::WITH_MBUFFER :
        {
            auto mbufptr = testcase.buffers_as_mbuffer();
            client->asyncWrite(msg, mbufptr, f);
            break;
        }
            
        case EchoTestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = testcase.buffers_as_buffer_chain();
            client->asyncWrite(msg, tmp, f);
            break;
        }
    }
    return client;
}

PostTest::PostTest(boost::asio::io_service& io, EchoTestcase& testcase): _io(io), _testcase(testcase)
{

}
void PostTest::handler(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)
{
    std::string sx = rdr->get_body_chain().to_string();
    std::string sy = _testcase.buffers_as_string();
#ifdef _VERBOSE
//        std::cout << "echo'ed body " << sx << std::endl;
//        std::cout << "testcasebody " << testcase.buffers_as_string() << std::endl;
#endif
    assert(rdr->statusCode() == 200);
    assert(sx == sy);
    ASSERT_EQ(rdr->statusCode(), 200);
    ASSERT_EQ(sx, sy);
    std::cout << "SUCCESS: " << _testcase._description <<  std::endl;
    
}

void PostTest::exec()
{
    auto url = _testcase._url;
    _client = std::shared_ptr<Client>(new Client(_io, url));
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    _msg->setMethod(HttpMethod::POST);

    auto f = std::bind(&PostTest::handler, this, std::placeholders::_1, std::placeholders::_2);

    switch(_testcase._test_type){
        case EchoTestType::WITH_STRING :
        {
            auto str_buf = _testcase.buffers_as_string();
            _client->asyncWrite(_msg, str_buf, f);
            break;
        }
            
        case EchoTestType::WITH_MBUFFER :
        {
            auto mbufptr = _testcase.buffers_as_mbuffer();
            _client->asyncWrite(_msg, mbufptr, f);
            break;
        }
            
        case EchoTestType::WITH_BUFFER_CHAIN :
        {
            auto tmp = _testcase.buffers_as_buffer_chain();
            _client->asyncWrite(_msg, tmp, f);
            break;
        }
    }

}

