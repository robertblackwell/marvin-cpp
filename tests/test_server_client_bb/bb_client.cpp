#include <doctest/doctest.h>
#include <json/json.hpp>
#include <marvin/external_src/trog/trog.hpp>
Trog_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/connection/connection.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/message/message_reader.hpp>

#include "bb_testcase.hpp"
#include "bb_client.hpp"

using namespace body_buffering;

using json = nlohmann::json;

TClient::TClient(boost::asio::io_service& io, std::string port, Testcase tc)
: m_io(io), m_scheme("http"), m_server("localhost"), m_port(port), m_testcase(tc), m_timer(m_io)
{
    m_conn_sptr = socketFactory(m_io, m_scheme, m_server, m_port);
}
void TClient::exec()
{
    LogDebug("");
    m_buffer_index = 0;
    m_test_cb = nullptr;
    connect();
}
void TClient::send_testcase_buffers(SysErrorCb cb)
{
    LogDebug("");
    m_buffer_index = 0;
    m_test_cb = cb;
    connect();
}

void TClient::connect()
{
    LogDebug("");
    m_conn_sptr->asyncConnect([this](Marvin::ErrorType& err, ISocket* conn) {
        LogDebug("connected");
        if( ! err ){
            m_rdr = std::make_shared<MessageReader>(m_io, m_conn_sptr);
            auto wbf = std::bind(&TClient::wait_before_write, this);
            m_io.post(wbf);
        } else {
            Marvin::ErrorType me = err;
            LogError("error_value", err.value(), " message: ", err.message());
            m_test_cb(me);
        }
    });
}

void TClient::write_line()
{
    LogDebug("");
    std::string line = m_testcase.lineAt(m_buffer_index);
    LogDebug(" line: ", line);
    if (line == "eof" ) {
        m_conn_sptr->shutdown();
        auto erok = Marvin::make_error_ok();
        std::size_t bytes = line.size();
        handle_write_complete(erok, bytes);
    } else if (line == "close") {
        m_conn_sptr->close();
        auto erok = Marvin::make_error_ok();
        std::size_t bytes = line.size();
        handle_write_complete(erok, bytes);
    } else {
        auto hf = std::bind(&TClient::handle_write_complete, this, std::placeholders::_1, std::placeholders::_2);
        m_conn_sptr->asyncWrite(line, hf);
    }
}
void TClient::handle_write_complete(Marvin::ErrorType& err, std::size_t bytes_transfered)
{
    LogDebug("");
    if( !err) {
        m_buffer_index++;
        if(m_buffer_index >= m_testcase.buffers().size()) {
            LogDebug("write complete start read");
            read_message();
        } else {
            write_line();
        }
    } else {
        Marvin::ErrorType err_val = err;
        m_test_cb(err_val);
    }
}
void TClient::wait_before_write()
{
    LogDebug("");
    m_timer.expires_from_now(boost::posix_time::milliseconds(100));
    m_timer.async_wait([this](const boost::system::error_code& err) {
        write_line();
    });

}

/**
* runs a test that reads reads a full message
*/
void TClient::read_message()
{
    LogDebug("getting started");
//        makeReader();
    auto h = std::bind(&TClient::onMessage, this, std::placeholders::_1);
    m_rdr->readMessage(h);
}

void TClient::onMessage(Marvin::ErrorType er)
{
    LogDebug("");
    Marvin::ErrorType expected_err = m_testcase.result_onheaders_err();
    std::string exp_s = Marvin::make_error_description(expected_err);
    std::string ers = Marvin::make_error_description(er);
    if( er != expected_err){
        std::cout << "bad" << std::endl;
    }
    assert(er == expected_err);
    REQUIRE(er == expected_err);
    if( er) {
        /// got the expected error code  - its an error so the response
        /// message will be meaningless
//        std::cout << "TestRunner::readMessage Success error as expected[" << ers << "] for testcase " << _testcase.getDescription() <<std::endl;
    } else {
        assert(m_rdr->statusCode() == m_testcase.result_status_code());
        REQUIRE(m_rdr->statusCode() == m_testcase.result_status_code());
        auto h1 = m_testcase.result_headers();
        auto h2 = m_rdr->getHeaders();
        bool hh = (h1 == h2);
        std::string raw_body = m_rdr->getContentBuffer()->to_string();
        json j = json::parse(raw_body);
        std::string req_body = j["req"]["body"];
        std::string ch_uuid = j["xtra"]["connection_handler_uuid"];
        std::string rh_uuid = j["xtra"]["request_handler_uuid"];

        std::string sx = m_rdr->getContentBuffer()->to_string();
        auto b1 = m_testcase.result_body();
        assert(b1 == req_body);
        REQUIRE(b1 == req_body);
        auto desc = m_testcase.getDescription();
        if(m_rdr->getHeader(Marvin::Http::HeadersV2::Connection) == Marvin::Http::HeadersV2::ConnectionClose) {
            // should close here - but client does not know how to do that cleanly.
            this->m_conn_sptr->close();
            this->m_rdr = nullptr;
        }
//        std::cout << "TestRunner::readMessage Success for testcase " << _testcase.getDescription() <<std::endl;
    }
}

#if 0
/**
* runs a test that reads the headers only
*/
void TClient::run_StreamingBodyRead()
{
    LogDebug("getting started");
//        makeReader();
    auto h = std::bind(&Testrunner::onHeaders, this, std::placeholders::_1);
    _rdr->readHeaders(h);

}
#endif



#if 0
void TClient::onBody(Marvin::ErrorType er, BufferChain chunk)
{
    LogDebug(" entry");
    // are we done - if not hang another read
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
    bool done = (er == Marvin::make_error_eom());
    body_accumulator += chain_to_string(chunk);
    if( done )
    {
        std::string expectedBody = _testcase.result_body();
        bool vb = _testcase.verify_body(body_accumulator);
        assert(vb);
        assert(er == Marvin::make_error_eom());
        REQUIRE(vb);
        REQUIRE(er == Marvin::make_error_eom());
        auto desc = _testcase.getDescription();
//        std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << _testcase.getDescription() <<std::endl;

    }else{
        _rdr->readBody(bh);
    }
    LogDebug("exit");
    
}
void Testrunner::onHeaders(Marvin::ErrorType er){
    LogDebug("entry");
    Marvin::ErrorType expected_err = _testcase.result_onheaders_err();
    std::string ers = er.message();
    assert(er == expected_err);
    assert(_rdr->statusCode() == _testcase.result_status_code());
    REQUIRE(er == expected_err);
    REQUIRE(_rdr->statusCode() == _testcase.result_status_code());
    auto h1 = _testcase.result_headers();
    auto h2 = _rdr->getHeaders();
    bool hhh = _testcase.verify_headers(h2);
    assert(hhh);
    REQUIRE(hhh);
    REQUIRE(h1 == h2);
    auto bh = std::bind(&Testrunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
//        std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << tcObj.getDescription() <<std::endl;
    _rdr->readBody(bh);
    LogDebug("exit");
}

#endif
