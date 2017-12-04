
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


