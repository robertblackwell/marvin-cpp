#include "pipeline.hpp"

Pipeline::Pipeline(boost::asio::io_service& io) : _io(io)
{
        _counter = 0;
        _client_sptr = std::shared_ptr<Client>(new Client(io, "http://whiteacorn.com/posts/rtw" ));
}
void Pipeline::setup()
{
    _msg_sptr = std::shared_ptr<MessageBase>(new MessageBase());
    _msg_sptr->setMethod(HttpMethod::GET);
    _msg_sptr->setHeader(HttpHeader::Name::Connection, "Keep-Alive");
    auto h = std::bind(&Pipeline::handler, this, _1, _2, _3);
    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [this](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
        this->handler(ec, rdr);
    };
    _client_sptr->asyncWrite(_msg_sptr, f);

}
void Pipeline::handler(Marvin::ErrorType err, MessageReaderSPtr rdr)
{
    if(!err) {
        MessageReaderSPtr b = _client_sptr->getResponse();
        BufferChain buf_chain = b->get_body_chain();
        std::string bdy = buf_chain.to_string();
        std::string sss = err.message();
//        std::cout << bdy << std::endl;
#if 0
        std::cout << "Successful message roundtrip counter: " << _counter << "http status: "
            << b->status() << " err: "
            << err.message() << std::endl;
#endif
        ASSERT_TRUE(b->statusCode() == 200 );
        if (_counter++ > 5)
            return;
        this->setup();
    } else {
        std::cout << "Failure Error : " << err.message() << std::endl;
    }
}
/**
* The test case
*/
TEST(ClientPipeline, Test01)
{
    boost::asio::io_service io;
    std::shared_ptr<Pipeline> pipeline_sptr = std::make_shared<Pipeline>(io);
    pipeline_sptr->setup();
    io.run();
}
