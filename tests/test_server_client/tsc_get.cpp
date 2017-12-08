#include "tsc_get.hpp"


GetTest::GetTest(boost::asio::io_service& io, EchoTestcase& testcase): _io(io), _testcase(testcase)
{

}
void GetTest::exec()
{
    _client = std::shared_ptr<Client>(new Client(_io, "http://localhost:9991" ));
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    std::function<void(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)> f = [this](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#if 1
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << _client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = _client->getResponse();
        BufferChain bdy_chain = b->get_body_chain();
        std::string body_as_string = bdy_chain.to_string();
        std::string bdy = body_as_string;
        auto st = b->statusCode();
        assert(b->statusCode() == 200);
        std::string code = _testcase.buffers_as_string();
        
#endif
        std::cout << "get request code = " << code << " success " << std::endl;
    };
    _client->asyncWrite(_msg, f);

}
