#include "forward_helpers.hpp"
#include "pipeline.hpp"

using namespace Marvin;
using namespace Marvin::Http;

Pipeline::Pipeline(boost::asio::io_service& io, Marvin::Uri& uri)
: m_io(io), m_uri(uri)
{
        m_counter = 0;
        m_max_counter = 5;
        m_client_sptr = std::shared_ptr<Client>(new Client(io, m_uri));
}
void Pipeline::setup()
{
    m_msg_sptr = std::shared_ptr<MessageBase>(new MessageBase());
    m_msg_sptr->setMethod(HttpMethod::GET);
    helpers::applyUriNonProxy(m_msg_sptr, m_uri);
    if( m_counter < m_max_counter - 1)
        m_msg_sptr->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionKeepAlive);
    else
        m_msg_sptr->setHeader(Marvin::Http::Headers::Name::Connection, Marvin::Http::Headers::Value::ConnectionClose);
    m_msg_sptr->setContent("");
//    auto h = std::bind(&Pipeline::handler, this, _1, _2, _3);
    std::function<void(Marvin::ErrorType& er, MessageReaderSPtr rdr)> f = [this](Marvin::ErrorType& ec, MessageReaderSPtr rdr) {
        this->handler(ec, rdr);
    };
    m_client_sptr->asyncWrite(m_msg_sptr, f);

}
void Pipeline::handler(Marvin::ErrorType err, MessageReaderSPtr rdr)
{
    if(!err) {
        MessageReaderSPtr b = m_client_sptr->getResponse();
        Marvin::BufferChainSPtr buf_chain_sptr = b->getContentBuffer();
        std::string bdy = buf_chain_sptr->to_string();
        std::string sss = err.message();
//        std::cout << bdy << std::endl;
#if 1
        std::cout << "Successful message pipeline counter: " << m_counter << " max_counter: " << m_max_counter << " http status: "
            << b->status() << " err: "
            << err.message() << std::endl;
#endif
        CHECK(b->statusCode() == 200 );
        if (m_counter++ > 5)
            return;
        this->setup();
    } else {
        std::cout << "Error : " << err.message() << " this is probably OK - the server should close the connection after the last response" << std::endl;
    }
}
/**
* The test case
*/
#if 1
TEST_CASE("ClientPipeline_Test01","")
{
    Marvin::Uri uri("http://whiteacorn.com/posts/rtw");
    boost::asio::io_service io;
    std::shared_ptr<Pipeline> pipeline_sptr = std::make_shared<Pipeline>(io, uri);
    pipeline_sptr->setup();
    io.run();
}
#endif
