#include "echo_smart.hpp"

#include <doctest/doctest.h>

#include <boost/algorithm/string.hpp>
#include <json/json.hpp>

#include <marvin/buffer/buffer_chain.hpp>

EchoSmart::EchoSmart(
            std::string path,   // the string that goes after the method usually for non proxy requests a relative path like /echo/smart
            HttpMethod  method,
            std::string scheme, // http or https
            std::string host,   // host name without the port so localhost not localhost:3000
            std::string port,    // port such as 3000){}
            std::string body 
)           
{
    m_path = path;
    m_scheme = scheme;
    m_method = method;
    m_host = host;
    m_port = port;
    m_body = body;
}
std::string EchoSmart::getHost() {return m_host;}
std::string EchoSmart::getPort() {return m_port;}
void EchoSmart::verifyResponse(Marvin::ErrorType& er, Marvin::MessageBaseSPtr response)
{
    CHECK(!er);
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    Marvin::BufferChainSPtr bsp = response->getContentBuffer();
    std::string raw_body = bsp->to_string();
    CHECK(response->status_code() == 200);

    nlohmann::json j;
    try{
        j = nlohmann::json::parse(raw_body);
    } catch(std::exception& e) {
        std::cout << "json error " << e.what() << std::endl;
        INFO("json parse failed");
        REQUIRE(false);
        return;
    }
    bool test_req = (j.find("req") != j.end());
    REQUIRE(test_req);      
    auto req_j = j["req"];
    bool test_junk = (req_j.find("junk") != req_j.end());
    bool test_headers = (req_j.find("headers") != req_j.end());
    bool test_headers_raw = (req_j.find("headers_raw") != req_j.end());
    bool test_method = (req_j.find("method") != req_j.end());
    bool test_uri = (req_j.find("uri") != req_j.end());
    bool test_body = (req_j.find("body") != req_j.end());
    bool test_trailers = (req_j.find("trailersbody") != req_j.end());
    REQUIRE(test_junk);
    REQUIRE(test_headers);
    REQUIRE(test_headers_raw);
    REQUIRE(test_method);
    REQUIRE(test_uri);
    REQUIRE(test_body);
    // REQUIRE(test_trailers);

    auto junk_j = req_j["junk"];
    auto headers_j = req_j["headers"];
    auto headers_raw_j = req_j["headers_raw"];
    auto method_j = req_j["method"];
    auto uri_j = req_j["uri"];
    auto body_j = req_j["body"];
    // auto trailers_j = req_j["trailers"];
    
    std::string junk_dump = junk_j.dump();
    std::string junk_value = junk_j.get<std::string>();
    CHECK(junk_value == std::string("some junk"));

    std::string headers_dump = headers_j.dump();
    std::map<std::string, std::string> header_values;
    for (nlohmann::json::iterator it = headers_j.begin(); it != headers_j.end(); ++it) {
        // std::cout << it.key() << " : " << it.value() << "\n";
        // servers can modify the upper/lower case of the headers keys
        std::string k = boost::to_upper_copy(it.key());
        std::string v = it.value(); //boost::to_upper_copy(it.value());

        // std::cout << k << " : " << v << "\n";

        header_values[k] = v;
    }
    std::string headers_raw = headers_raw_j.dump();
    std::string headers_raw_value = headers_raw_j.get<std::string>();
    
    std::string method_dump = method_j.dump();
    std::string method_value = method_j.get<std::string>();
    std::string tmp = httpMethodString(m_method);
    CHECK(method_value == tmp);// std::string("POST"));

    std::string uri_dump = uri_j.dump();
    std::string uri_value = uri_j.get<std::string>();
    CHECK(uri_value == m_path);

    std::string body_dump = body_j.dump();
    std::string body_value = body_j.get<std::string>();
    CHECK(body_value == m_body);
    // std::string trailers_dump = trailers_j.dump();
    bool thdr_content_length_exist = (header_values.find("CONTENT-LENGTH") != header_values.end());
    bool thdr_accept_exist = (header_values.find("ACCEPT") != header_values.end());
    bool thdr_accept_charset_exist = (header_values.find("ACCEPT-CHARSET") != header_values.end());
    bool thdr_accept_language_exist = (header_values.find("ACCEPT-LANGUAGE") != header_values.end());
    bool thdr_user_agent_exist = (header_values.find("USER-AGENT") != header_values.end());
    bool thdr_connection_exist = (header_values.find("CONNECTION") != header_values.end());
    bool thdr_xspecial_header_exist = (header_values.find("X-SPECIAL-HEADER") != header_values.end());

    CHECK(thdr_content_length_exist);
    CHECK(thdr_accept_exist);
    CHECK(thdr_accept_charset_exist);
    CHECK(thdr_accept_language_exist);
    CHECK(thdr_user_agent_exist);
    CHECK(thdr_connection_exist);
    CHECK(thdr_xspecial_header_exist);

    std::string hdrv_content_length = header_values["CONTENT-LENGTH"];
    std::string hdrv_accept = header_values["ACCEPT"];
    std::string hdrv_accept_charset = header_values["ACCEPT-CHARSET"];
    std::string hdrv_accept_language = header_values["ACCEPT-LANGUAGE"];
    std::string hdrv_user_agent = header_values["USER-AGENT"];
    std::string hdrv_connection = header_values["CONNECTION"];
    std::string hdrv_xspecial = header_values["X-SPECIAL-HEADER"];

    Marvin::HeadersV2 original_headers = this->m_request_sptr->headers();

    std::string orig_content_length = original_headers.atKey("CONTENT-LENGTH").get();
    std::string orig_accept = original_headers.atKey("ACCEPT").get();
    std::string orig_accept_charset = original_headers.atKey("ACCEPT-CHARSET").get();
    std::string orig_accept_language = original_headers.atKey("ACCEPT-LANGUAGE").get();
    std::string orig_user_agent = original_headers.atKey("USER-AGENT").get();
    std::string orig_connection = original_headers.atKey("CONNECTION").get();
    std::string orig_xspecial = original_headers.atKey("X-SPECIAL-HEADER").get();

    CHECK(hdrv_accept == orig_accept);
    CHECK(hdrv_accept_charset == orig_accept_charset);
    CHECK(hdrv_accept_language == orig_accept_language);
    CHECK(hdrv_connection == orig_connection);
    CHECK(hdrv_content_length == orig_content_length);
    CHECK(hdrv_user_agent == orig_user_agent);
    CHECK(hdrv_xspecial == orig_xspecial);

    return;
}
Marvin::BufferChainSPtr EchoSmart::makeBody()
{
    using namespace Marvin;
    BufferChainSPtr chain_sptr  = BufferChain::makeSPtr(m_body);
    return chain_sptr;
}
Marvin::MessageBaseSPtr EchoSmart::makeRequest()
{
    /// this sends the request to our mitm proxy
    Marvin::MessageBaseSPtr msg = std::make_shared<Marvin::MessageBase>();
    msg->method(m_method);
    msg->target(m_path);
    msg->header(Marvin::HeadersV2::Host, m_host);
    msg->header("User-Agent","Opera/9.80 (X11; Linux x86_64; Edition Next) Presto/2.12.378 Version/12.50");
    msg->header(
        "Accept","text/html, application/xml;q=0.9, application/xhtml xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1");
    msg->header("Accept-Language","en");
    msg->header("Accept-Charset","iso-8859-1, utf-8, utf-16, utf-32, *;q=0.1");
    msg->header(Marvin::HeadersV2::AcceptEncoding,"deflate, gzip, x-gzip, identity, *;q=0");

    msg->header(Marvin::HeadersV2::Connection,"Close");
    msg->header(Marvin::HeadersV2::ETag,"1928273tefadseercnbdh");
    msg->header("X-SPECIAL-HEADER", "proof of passthru");
    // std::string s = "012345678956";
    // Marvin::BufferChainSPtr bdy = Marvin::BufferChain::makeSPtr(s);
    // msg->setContent(bdy);
    m_request_sptr = msg;
    return msg;
}
