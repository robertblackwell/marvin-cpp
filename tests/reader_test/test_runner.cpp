
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)
#include "test_runner.hpp"

std::string chain_to_string(BufferChain chain)
{
    return chain.to_string();
}

void Testrunner::makeReader()
{
    rdr_ = std::shared_ptr<MessageReaderV2>(new MessageReaderV2(io_, conn_));
    auto rr = new MessageReaderV2(io_, conn_);
}
/**
* Constructor - tcIndex is an index into the set of testcases
* that the class TestCases knows about
*/
Testrunner::Testrunner(boost::asio::io_service& io, ReadSocketInterfaceSPtr rd_sock, Testcase tcObj)
    : io_(io),
    _tcObj(tcObj)
{
    LogDebug("");
    conn_ = rd_sock;
    rdr_ = std::shared_ptr<MessageReaderV2>(new MessageReaderV2(io_, conn_));
    body = std::string("");
    bodyStream.str(body);
    body_accumulator = "";
}
Testrunner::~Testrunner()
{
    LogDebug("");
}
void Testrunner::onMessage(Marvin::ErrorType er)
{
    LogDebug("");
    Marvin::ErrorType expected_err = _tcObj.result_onheaders_err();
    std::string exp_s = expected_err.message();
    std::string ers = er.message();
    if( er != expected_err){
        std::cout << "bad" << std::endl;
    }
//        assert(er == expected_err);
    assert(rdr_->statusCode() == _tcObj.result_status_code());
    REQUIRE(rdr_->statusCode() == _tcObj.result_status_code());
    auto h1 = _tcObj.result_headers();
    auto h2 = rdr_->getHeaders();
    bool hh = (h1 == h2);
    assert(_tcObj.result_headers() == rdr_->getHeaders());
    REQUIRE(_tcObj.result_headers() == rdr_->getHeaders());
    auto b1 = _tcObj.result_body();
    auto b2 = rdr_->get_body_chain();
    auto b3 = rdr_->get_raw_body_chain();
    auto s2 = chain_to_string(b2);
    auto s3 = chain_to_string(b3);
    assert(b1 == s2);
    REQUIRE(b1 == s2);
    auto desc = _tcObj.getDescription();
    std::cout << "TestRunner::readMessage Success for testcase " << _tcObj.getDescription() <<std::endl;
}
void Testrunner::onBody(Marvin::ErrorType er, BufferChain chunk)
{
    LogDebug(" entry");
    // are we done - if not hang another read
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
    bool done = (er == Marvin::make_error_eom());
    body_accumulator += chain_to_string(chunk);
    if( done )
    {
        std::string expectedBody = _tcObj.result_body();
        bool vb = _tcObj.verify_body(body_accumulator);
        assert(vb);
        assert(er == Marvin::make_error_eom());
        REQUIRE(vb);
        REQUIRE(er == Marvin::make_error_eom());
        auto desc = _tcObj.getDescription();
        std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << _tcObj.getDescription() <<std::endl;

    }else{
        rdr_->readBody(bh);
    }
    LogDebug("exit");
    
}
void Testrunner::onHeaders(Marvin::ErrorType er){
    LogDebug("entry");
    Marvin::ErrorType expected_err = _tcObj.result_onheaders_err();
    std::string ers = er.message();
    assert(er == expected_err);
    assert(rdr_->statusCode() == _tcObj.result_status_code());
    REQUIRE(er == expected_err);
    REQUIRE(rdr_->statusCode() == _tcObj.result_status_code());
    auto h1 = _tcObj.result_headers();
    auto h2 = rdr_->getHeaders();
    bool hhh = _tcObj.verify_headers(h2);
    assert(hhh);
    REQUIRE(hhh);
    REQUIRE(h1 == h2);
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
//        std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << tcObj.getDescription() <<std::endl;
    rdr_->readBody(bh);
    LogDebug("exit");
}
/**
* runs a test that reads reads a full message
*/
void Testrunner::run_FullMessageRead()
{
    LogDebug("getting started");
//        makeReader();
    auto h = std::bind(&Testrunner::onMessage, this, std::placeholders::_1);
    rdr_->readMessage(h);
}
/**
* runs a test that reads the headers only
*/
void Testrunner::run_StreamingBodyRead()
{
    LogDebug("getting started");
//        makeReader();
    auto h = std::bind(&Testrunner::onHeaders, this, std::placeholders::_1);
    rdr_->readHeaders(h);

}

