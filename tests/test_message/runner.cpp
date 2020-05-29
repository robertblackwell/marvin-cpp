
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>

#include "runner.hpp"

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
    m_conn = rd_sock;
    m_rdr = std::make_shared<Marvin::MessageReader>(m_conn);
    m_body = std::string("");
    m_bodyStream.str(m_body);
    m_body_accumulator = "";
}
Testrunner::~Testrunner()
{
}
void Testrunner::onMessage(Marvin::ErrorType er)
{
    Marvin::ErrorType expected_err = m_tcObj.result_onheaders_err();
    std::string exp_s = expected_err.message();
    std::string ers = er.message();
    if( er != expected_err){
        std::cout << "bad" << std::endl;
    }
    REQUIRE(m_rdr->status_code() == m_tcObj.result_status_code());
    auto h1 = m_tcObj.result_headers();
    auto h2 = m_rdr->headers();
    bool hh = h1.sameValues(h2);

    if( m_tcObj.result_headers().sameValues(m_rdr->headers()))
        assert(m_tcObj.result_headers().sameValues(m_rdr->headers()));

    bool catch_is_stupid = m_tcObj.result_headers().sameValues(m_rdr->headers());
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
    m_rdr->readMessage([this](Marvin::ErrorType err)
    {
        Marvin::MessageBase msg{*m_rdr};
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

