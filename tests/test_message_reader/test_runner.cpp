
#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
#include "test_runner.hpp"

#include <doctest/doctest.h>



std::string chain_to_string(Marvin::BufferChain chain)
{
    return chain.to_string();
}

void Testrunner::makeReader()
{
    m_rdr = std::shared_ptr<Marvin::MessageReader>(new Marvin::MessageReader(m_io, m_conn));
    auto rr = new Marvin::MessageReader(m_io, m_conn);
}
/**
* Constructor - tcIndex is an index into the set of testcases
* that the class TestCases knows about
*/
Testrunner::Testrunner(boost::asio::io_service& io, Marvin::ISocketSPtr rd_sock, Testcase tcObj)
    : m_io(io),
    m_tcObj(tcObj)
{
    TROG_DEBUG("");
    m_conn = rd_sock;
    m_rdr = std::shared_ptr<Marvin::MessageReader>(new Marvin::MessageReader(m_io, m_conn));
    m_body = std::string("");
    m_bodyStream.str(m_body);
    m_body_accumulator = "";
}
Testrunner::~Testrunner()
{
    TROG_DEBUG("");
}
void Testrunner::onMessage(Marvin::ErrorType er)
{
    TROG_DEBUG("");
    Marvin::ErrorType expected_err = m_tcObj.result_onheaders_err();
    std::string exp_s = expected_err.message();
    std::string ers = er.message();
    if( er != expected_err){
        std::cout << "bad" << std::endl;
    }
    REQUIRE(m_rdr->statusCode() == m_tcObj.result_status_code());
    auto h1 = m_tcObj.result_headers();
    auto h2 = m_rdr->getHeaders();
    bool hh = h1.sameValues(h2);

    if( m_tcObj.result_headers().sameValues(m_rdr->getHeaders()))
        assert(m_tcObj.result_headers().sameValues(m_rdr->getHeaders()));

    bool catch_is_stupid = m_tcObj.result_headers().sameValues(m_rdr->getHeaders());
    REQUIRE( catch_is_stupid );
    
    auto b1 = m_tcObj.result_body();
    auto b2 = m_rdr->getContentBuffer();
    auto s2 = b2->to_string();
    REQUIRE(b1 == s2);
    auto desc = m_tcObj.getDescription();
}
void Testrunner::onBody(Marvin::ErrorType er, Marvin::BufferChainSPtr chunkSPtr)
{
    TROG_DEBUG(" entry");
    // are we done - if not hang another read
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
    bool done = (er == Marvin::make_error_eom());
    m_body_accumulator += chunkSPtr->to_string();
    if( done )
    {
        std::string expectedBody = m_tcObj.result_body();
        bool vb = m_tcObj.verify_body(m_body_accumulator);
        REQUIRE(vb);
        REQUIRE(er == Marvin::make_error_eom());
        auto desc = m_tcObj.getDescription();

    }else{
        m_rdr->readBody(bh);
    }
    TROG_DEBUG("exit");
    
}
void Testrunner::onHeaders(Marvin::ErrorType er){
    TROG_DEBUG("entry");
    Marvin::ErrorType expected_err = m_tcObj.result_onheaders_err();
    std::string ers = er.message();
    assert(er == expected_err);
    REQUIRE(er == expected_err);
    REQUIRE(m_rdr->statusCode() == m_tcObj.result_status_code());
    auto h1 = m_tcObj.result_headers();
    auto h2 = m_rdr->getHeaders();
    bool hhh = m_tcObj.verify_headers(h2);
    //assert(hhh);
    REQUIRE(hhh);
    bool catch_is_stupid = h1.sameValues(h2);
    REQUIRE( catch_is_stupid );
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
//        std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << tcObj.getDescription() <<std::endl;
    m_rdr->readBody(bh);
    TROG_DEBUG("exit");
}
/**
* runs a test that reads reads a full message
*/
void Testrunner::run_FullMessageRead()
{
    TROG_DEBUG("getting started");
//        makeReader();
    auto h = std::bind(&Testrunner::onMessage, this, std::placeholders::_1);
    m_rdr->readMessage(h);
}
/**
* runs a test that reads the headers only
*/
void Testrunner::run_StreamingBodyRead()
{
    TROG_DEBUG("getting started");
//        makeReader();
    auto h = std::bind(&Testrunner::onHeaders, this, std::placeholders::_1);
   m_rdr->readHeaders(h);

}

