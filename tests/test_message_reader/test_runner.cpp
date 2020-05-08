
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
    m_rdr = std::make_shared<Marvin::MessageReader>(m_conn);
    auto rr = new Marvin::MessageReader(m_conn);
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
    m_rdr = std::make_shared<Marvin::MessageReader>(m_conn);
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
/**
* runs a test that reads reads a full message
*/
void Testrunner::run_FullMessageRead()
{
    TROG_DEBUG("getting started");
    m_rdr->readMessage([this](Marvin::ErrorType err)
    {
        onMessage(err);
    });
}
/**
 * Initiate a multiple message read
 */ 
void Testrunner:: run_MultipleMessageRead()
{
    m_rdr->readMessage([this](Marvin::ErrorType err)
    {
        onMessage(err);
        if(err) {
            return;
        }
        run_MultipleContinue();
    });
}
void Testrunner::run_MultipleContinue()
{
    run_MultipleMessageRead();
}

/**
* runs a test that reads the headers only
*/
void Testrunner::run_StreamingBodyRead()
{
    throw ("streaming body test not implemented");
}

