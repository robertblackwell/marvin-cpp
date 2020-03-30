#include "bf_get.hpp"

using namespace body_format;

GetTest::GetTest(boost::asio::io_service& io, Testcase& testcase): _io(io), _testcase(testcase)
{

}
void GetTest::exec()
{
    _client = std::shared_ptr<Client>(new Client(_io, "http://localhost:9991" ));
    _msg = std::shared_ptr<MessageBase>(new MessageBase());
    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [this](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
#if 1
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

//        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
//        std::cout << "request " << std::hex << _client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderSPtr b = _client->getResponse();
        Marvin::BufferChainSPtr bdy_chain_sptr = b->getBody();
        std::string body_as_string = bdy_chain_sptr->to_string();
        std::string bdy = body_as_string;
        auto st = b->statusCode();
        assert(b->statusCode() == 200);
        std::string code = _testcase.buffers_as_string();
#pragma clang diagnostic pop

#endif
//        std::cout << "get request code = " << code << " success " << std::endl;
    };
    _client->asyncWrite(_msg, f);

}
