#include <doctest/doctest.h>
#include <json/json.hpp>
#include <marvin/http/message_factory.hpp>
#include "test_headers.hpp"
#include <marvin/server/server_connection_manager.hpp>
#include "tp_tunnel.hpp"

using namespace tp;
using json = nlohmann::json;
namespace tp {
Tunnel::Tunnel(boost::asio::io_service& io, tp::TestcaseSPtr testcaseSPtr): m_io(io), m_testcase_sptr(testcaseSPtr)
{
    m_msg_sptr = m_testcase_sptr->m_msg_sptr;
    m_scheme = m_testcase_sptr->m_proxy_scheme;
    m_proxy_server = m_testcase_sptr->m_proxy_host;
    m_proxy_port = m_testcase_sptr->m_proxy_port;
}
void Tunnel::handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    Marvin::BufferChainSPtr bsp = rdr->getContentBuffer();
    std::string raw_body = bsp->to_string();
    /// check correct status
    CHECK(rdr->statusCode() == 200);

    /// now test echo correctly
    json j;
    /// the content of the respons should be json text
    try{
        j = json::parse(raw_body);
    } catch(std::exception& e) {
        std::cout << "json error " << e.what() << std::endl;
        INFO("json parse failed");
        CHECK(false);
        return;
    }
    auto ra = j["req"];
    ///
    /// ///////////////////////////////////////////////////////////////////////////////////
    /// @warn - if any of the stuff below throws an exception first check that the server
    /// has sent back a json string that is correctly formated - the most likely problem
    /// is that the json_encode of the headers has not been done correctly
    /// ///////////////////////////////////////////////////////////////////////////////////
    ///
    /// check method
    auto echoed_method = j["req"]["method"].get<std::string>();
    auto original_method = m_testcase_sptr->m_msg_sptr->getMethodAsString();
    CHECK( (echoed_method == original_method) );
    /// check headers
    auto echoed_headers = test::helpers::headersFromJson(j["req"]["headers"]);
    auto original_headers = m_testcase_sptr->m_msg_sptr->getHeaders();
    /// these are the headers that should be preserved - they arer [present in every test case just for convenience
    CHECK( (echoed_headers["ACCEPT"] == original_headers["ACCEPT"]) );
    CHECK( (echoed_headers["ACCEPT-CHARSET"] == original_headers["ACCEPT-CHARSET"]) );
    CHECK( (echoed_headers["ACCEPT-LANGUAGE"] == original_headers["ACCEPT-LANGUAGE"]) );
    CHECK( (echoed_headers[Marvin::Http::Headers::Name::ContentLength] == original_headers[Marvin::Http::Headers::Name::ContentLength]) );
    CHECK( (echoed_headers[Marvin::Http::Headers::Name::Host] == original_headers[Marvin::Http::Headers::Name::Host]) );
    CHECK( (echoed_headers["USER-AGENT"] == original_headers["USER-AGENT"]) );
    /// these headers should be present but changed
    CHECK(echoed_headers.has(Marvin::Http::Headers::Name::Connection));
    /// the proxy only allows connection close
    CHECK(echoed_headers.get(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionClose);

    CHECK(echoed_headers.has(Marvin::Http::Headers::Name::AcceptEncoding));
    CHECK(echoed_headers.has(Marvin::Http::Headers::Name::TE));

    /// check body
    std::string echoedBody = j["req"]["body"];
    std::string originalBody = (m_testcase_sptr->m_msg_sptr->getContentBuffer())->to_string();
    CHECK(echoedBody == originalBody);

    if(rdr->getHeader(Marvin::Http::Headers::Name::Connection) == Marvin::Http::Headers::Value::ConnectionClose) {
        m_client_sptr->close();
        m_client_sptr = nullptr;
    }
    
//    std::cout << "Tunnel::" << _testcase._description << std::endl;
}
void Tunnel::exec()
{
    
    m_client_sptr = std::shared_ptr<Client>(new Client(m_io, m_scheme, m_proxy_server, m_proxy_port));
    m_connect_msg_sptr = std::make_shared<Marvin::Http::MessageBase>();
    Marvin::Http::makeProxyConnectRequest(*m_connect_msg_sptr, m_proxy_server, m_proxy_port);
    auto f = std::bind(&Tunnel::p_connect_handler, this, std::placeholders::_1, std::placeholders::_2);
    std::string buf("");
    m_client_sptr->asyncWrite(m_connect_msg_sptr, buf, f);
}
void Tunnel::p_connect_handler(Marvin::ErrorType& er, MessageReaderSPtr rdr)
{
    Marvin::BufferChainSPtr bsp = rdr->getContentBuffer();
    std::string raw_body = bsp->to_string();
    /// check correct status
    REQUIRE(!er);
    CHECK(rdr->statusCode() == 200);
    p_send_request();
}
void Tunnel::p_send_request()
{
    /// do not want to create a new Client unless we can do it with an already open connection
//    m_client_sptr = std::shared_ptr<Client>(new Client(m_io, m_scheme, m_proxy_server, m_proxy_port));

    auto f = std::bind(&Tunnel::handler, this, std::placeholders::_1, std::placeholders::_2);

    /// generate a random string and use it as the message content
    boost::uuids::uuid tmp_uuid = boost::uuids::random_generator()();
    std::string tmps = boost::uuids::to_string(tmp_uuid);
    m_msg_sptr->setContent(tmps);
    m_testcase_sptr->m_msg_sptr->setContent(tmps);

    auto buf = m_testcase_sptr->m_msg_sptr->getContentBuffer();

    m_client_sptr->asyncWrite(m_msg_sptr, buf, f);

}

} // namespace
