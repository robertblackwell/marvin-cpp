
// Sample custom iterator.
// By perfectly.insane (http://www.dreamincode.net/forums/index.php?showuser=76558)
// From: http://www.dreamincode.net/forums/index.php?showtopic=58468

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <regex>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <json/json.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/http/headers_v2.hpp>

using namespace Marvin;

///
/// KeepAlive is true if:
///     there is a connection header that contains the string "[ ,]keep-alive[ ,]" case independent
///     or
///     there is NOT a connection header that contain the string 'close' case insensitive
///         and the msg http version is 1.1
///
/// Keepalive is explicitly false
///     there is a connection header that contain the string 'close' case insensitive
///     or
///     there is NOT a connection header that contain the string 'keep-alive' case insensitive
///         and the msg http version is 1.0
///

// std::regex r_close(R"(\s*,\s*|\s*;\s*|\s*)close(\s*,\s*|\s*;\s*|\s*))", std::regex::icase);
// std::regex r_keep_alive(R"(\s*,\s*|\s*;\s*|\s*)keep-malive(\s*,\s*|\s*;\s*|\s*))", std::regex::icase);
void testHeaderKeepAlive(std::string test_value, bool expected)
{
    HeadersV2 h1_1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", test_value},
        {"11","1111111"},
        {"22","2222222"}
    }};
    CHECK( (expected == isConnectionKeepAlive(h1_1)));
}
void testHeaderConnectionClose(std::string test_value, bool expected)
{
    HeadersV2 h1_1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", test_value},
        {"11","1111111"},
        {"22","2222222"}
    }};
    CHECK( (expected == isConnectionClose(h1_1)));
}
void testMessageConnectionKeepAlive(std::string test_value, int minor_version, bool expected)
{
    MessageBase msg;
    msg.version_minor(minor_version);
    msg.header("bb","BBBBB");
    msg.header("ccc", "CCCCCC");
    msg.header("11","1111111");
    msg.header("22","2222222");
    msg.header("Connection", test_value);
    msg.header("11","1111111");
    msg.header("22","2222222");

    CHECK( (expected == isConnectionKeepAlive(msg)));
}
void testMessageConnectionClose(std::string test_value, int minor_version, bool expected)
{
    MessageBase msg;
    msg.version_minor(minor_version);
    msg.header("bb","BBBBB");
    msg.header("ccc", "CCCCCC");
    msg.header("11","1111111");
    msg.header("22","2222222");
    msg.header("Connection", test_value);
    msg.header("11","1111111");
    msg.header("22","2222222");

    CHECK( (expected != isConnectionKeepAlive(msg)));
}


TEST_CASE("headers_add_remove")
{
    HeadersV2 headers;
    CHECK(headers.size() == 0);
    headers.setAtKey("Connection", "keep-alive");
    CHECK(headers.size() == 1);
    CHECK( ( !!headers.atKey(HeadersV2::Connection )) );
    CHECK( ( !!headers.atKey("Connection")) );
    CHECK( ( !!headers.atKey("conNecTion")) );
    CHECK( ( !!headers.atKey("CONNECTION")) );
    CHECK(headers.atKey("Connection").get() == "keep-alive");
    CHECK(headers.findAtIndex("Connection").get() == 0);
    auto it1 = headers.find("connection");
    auto it2 = headers.end();
    CHECK( !(headers.find("connection") == headers.end()) );

    headers.setAtKey("Content-Length", "33");
    CHECK(headers.size() == 2);
    CHECK( ( !!headers.atKey(HeadersV2::ContentLength )) );
    CHECK( ( !!headers.atKey("Content-length")) );
    CHECK(headers.findAtIndex("content-length").get() == 1);
    CHECK( !!(headers.find("content-length") != headers.end()) );

    CHECK( (! headers.atKey("some-other-key")) );
    CHECK( (! headers.findAtIndex("someother-keyvalue")) );
    CHECK( (headers.find("someother-key") == headers.end()) );
    
    headers.removeAtKey("connection");
    CHECK(headers.size() == 1);
    CHECK( ( !headers.atKey("CONNECTION")) );
    CHECK( ( !headers.findAtIndex("CONNECTION")) );
    CHECK( (headers.find("connection") == headers.end()) );
    CHECK( ( !!headers.atKey("COntent-length")) );
    CHECK( ( !!headers.findAtIndex("COntent-length")) );
    CHECK( !!(headers.find("content-length") != headers.end()) );

    headers.removeAtKey("content-length");
    CHECK(headers.size() == 0);
    CHECK( ( !headers.atKey("COntent-length")) );
    CHECK( ( !headers.findAtIndex("COntent-length")) );
    CHECK( !!(headers.find("content-length") == headers.end()) );

}
TEST_CASE("headers_same")
{
    HeadersV2 h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeadersV2 h1_dup{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeadersV2 h1_reversed{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"44","4444444"},
        {"33","3333333"},
        {"22","2222222"},
        {"11","1111111"},
        {"ccc", "CCCCCC"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
    }};

    CHECK(h1.sameValues(h1_dup));
    CHECK(h1.sameValues(h1_reversed));

    CHECK(h1.sameOrderAndValues(h1_dup));
    CHECK( ! h1.sameOrderAndValues(h1_reversed));
}

TEST_CASE("header_iskeep_alive")
{
    HeadersV2 h1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", "Keep-AliVe, another1, another2"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    CHECK(isConnectionKeepAlive(std::string(" this has keep-alive embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this has Keep-alive embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this, has, Keep-Alive, embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this, has, KEEP-ALIVE, embedded")));
    CHECK(! isConnectionKeepAlive(std::string(" this, has, Close, embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this has ,keep-alive, embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this has,Keep-alive,embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this, has, Keep-Alive , embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this, has, KEEP-ALIVE     embedded")));
    CHECK(! isConnectionKeepAlive(std::string(" this, has, keep-aliveembedded")));
    CHECK(! isConnectionKeepAlive(std::string(" this, haskeepalive, embedded")));
    CHECK(isConnectionKeepAlive(std::string(" this, has, keep-alive;, embedded")));
    CHECK(isConnectionKeepAlive(std::string("keep-alive;, embedded")));
    CHECK(isConnectionKeepAlive(std::string("thi keep-alive")));
    CHECK(isConnectionKeepAlive(std::string("keep-alive")));

    testHeaderKeepAlive(" this has keep-alive embedded", true); 
    testHeaderKeepAlive(" this, has, Keep-Alive, embedded", true);
    testHeaderKeepAlive(" this, has, KEEP-ALIVE, embedded", true);
    testHeaderKeepAlive(" this, has, Close, embedded", false);
    testHeaderKeepAlive(" this has ,keep-alive, embedded", true);
    testHeaderKeepAlive(" this has,Keep-alive,embedded", true);
    testHeaderKeepAlive(" this, has, Keep-Alive , embedded", true);
    testHeaderKeepAlive(" this, has, KEEP-ALIVE     embedded", true);
    testHeaderKeepAlive(" this, has, keep-aliveembedded", false);
    testHeaderKeepAlive(" this, haskeepalive, embedded", false);
    testHeaderKeepAlive(" this, has, keep-alive;, embedded", true);
    testHeaderKeepAlive("keep-alive;, embedded", true);
    testHeaderKeepAlive("thi keep-alive", true);
    testHeaderKeepAlive("keep-alive", true);
}
TEST_CASE("header_connectionclose")
{

    CHECK(isConnectionClose(std::string(" this has close embedded")));
    CHECK(isConnectionClose(std::string(" this has close embedded")));
    CHECK(isConnectionClose(std::string(" this, has, cloSe, embedded")));
    CHECK(isConnectionClose(std::string(" this, has, CLOSE, embedded")));
    CHECK(! isConnectionClose(std::string(" this, has, keep-alive, embedded")));
    CHECK(isConnectionClose(std::string(" this has ,close, embedded")));
    CHECK(isConnectionClose(std::string(" this has,Close,embedded")));
    CHECK(isConnectionClose(std::string(" this, has, Close , embedded")));
    CHECK(isConnectionClose(std::string(" this, has, CLOSE     embedded")));
    CHECK(! isConnectionClose(std::string(" this, has, closeembedded")));
    CHECK(! isConnectionClose(std::string(" this, hasclose, embedded")));
    CHECK(isConnectionClose(std::string(" this, has, close;, embedded")));
    CHECK(isConnectionClose(std::string("close;, embedded")));
    CHECK(isConnectionClose(std::string("thi close")));
    CHECK(isConnectionClose(std::string("close")));

    testHeaderConnectionClose(" this has close embedded" , true);
    testHeaderConnectionClose(" this has close embedded" , true);
    testHeaderConnectionClose(" this, has, cloSe, embedded" , true);
    testHeaderConnectionClose(" this, has, CLOSE, embedded" , true);
    testHeaderConnectionClose(" this, has, keep-alive, embedded" , false);
    testHeaderConnectionClose(" this has ,close, embedded" , true);
    testHeaderConnectionClose(" this has,Close,embedded" , true);
    testHeaderConnectionClose(" this, has, Close , embedded" , true);
    testHeaderConnectionClose(" this, has, CLOSE     embedded" , true);
    testHeaderConnectionClose(" this, has, closeembedded" , false);
    testHeaderConnectionClose(" this, hasclose, embedded" , false);
    testHeaderConnectionClose(" this, has, close;, embedded" , true);
    testHeaderConnectionClose("close;, embedded" , true);
    testHeaderConnectionClose("thi close" , true);
    testHeaderConnectionClose("close" , true);

}
TEST_CASE("message_connection_keep_alive_1")
{
    testMessageConnectionKeepAlive(" this has keep-alive embedded", 1, true);
}
TEST_CASE("message_connection_keep_alive_2")
{
    testMessageConnectionKeepAlive(" this, has, embedded", 1, true);
    std::cout << " " << std::endl;
}
TEST_CASE("message_connection_keep_alive_3")
{
    testMessageConnectionKeepAlive(" this, has, close", 1, false);
    std::cout << " " << std::endl;
}
TEST_CASE("message_connection_keep_alive_4")
{
    testMessageConnectionKeepAlive(" this, has, close", 0, false);
    std::cout << " " << std::endl;
}
TEST_CASE("message_connection_keep_alive")
{
    testMessageConnectionKeepAlive(" this, has, embedded", 0, false);
    std::cout << " " << std::endl;
}
TEST_CASE("copy constructor test")
{
    HeadersV2 h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeadersV2 h2{h1};
    std::string bb = "bb";
    bool x = !!h2.atKey(bb);
    bool y = !!h1.atKey(bb);
    CHECK(x);
    CHECK(y);
    CHECK((h1.size() == h2.size()));
    HeadersV2 h3{};
    h3 = h1;
    std::string ccc = "ccc";
    CHECK((h3.size() == h1.size()));
    CHECK(!!h1.atKey(ccc));
    CHECK(!!h3.atKey(ccc));
}
TEST_CASE("move test")
{
    HeadersV2 h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    std::size_t h1_initial_size = h1.size();
    HeadersV2 h2{std::move(h1)};
    std::string bb = "bb";
    bool x = !!h2.atKey(bb);
    bool y = !!h1.atKey(bb);

    CHECK(x);
    CHECK(!y);
    CHECK((h1.size() == 0));
    CHECK((h2.size() == h1_initial_size));
    HeadersV2 h3{};
    h3 = std::move(h2);
    std::string ccc = "ccc";
    CHECK(!h1.atKey(ccc));
    CHECK(!h2.atKey(ccc));
    CHECK((h2.size() == 0));
    CHECK((h3.size() == h1_initial_size));
    CHECK(!!h3.atKey(ccc));
}