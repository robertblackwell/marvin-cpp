
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
    HeaderFields h1_1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", test_value},
        {"11","1111111"},
        {"22","2222222"}
    }};
    CHECK( (expected == is_connection_keep_alive(h1_1)));
}
void testHeaderConnectionClose(std::string test_value, bool expected)
{
    HeaderFields h1_1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", test_value},
        {"11","1111111"},
        {"22","2222222"}
    }};
    CHECK( (expected == is_connection_close(h1_1)));
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

    CHECK( (expected == is_connection_keep_alive(msg)));
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

    CHECK( (expected != is_connection_keep_alive(msg)));
}


TEST_CASE("headers_add_remove")
{
    HeaderFields headers;
    CHECK(headers.size() == 0);
    headers.set_at_key("Connection", "keep-alive");
    CHECK(headers.size() == 1);
    CHECK( ( !!headers.at_key(HeaderFields::Connection )) );
    CHECK( ( !!headers.at_key("Connection")) );
    CHECK( ( !!headers.at_key("conNecTion")) );
    CHECK( ( !!headers.at_key("CONNECTION")) );
    CHECK(headers.at_key("Connection").get() == "keep-alive");
    CHECK(headers.find_at_index("Connection").get() == 0);
    auto it1 = headers.find("connection");
    auto it2 = headers.end();
    CHECK( !(headers.find("connection") == headers.end()) );

    headers.set_at_key("Content-Length", "33");
    CHECK(headers.size() == 2);
    CHECK( ( !!headers.at_key(HeaderFields::ContentLength )) );
    CHECK( ( !!headers.at_key("Content-length")) );
    CHECK(headers.find_at_index("content-length").get() == 1);
    CHECK( !!(headers.find("content-length") != headers.end()) );

    CHECK( (! headers.at_key("some-other-key")) );
    CHECK( (!headers.find_at_index("someother-keyvalue")) );
    CHECK( (headers.find("someother-key") == headers.end()) );

    headers.remove_at_key("connection");
    CHECK(headers.size() == 1);
    CHECK( ( !headers.at_key("CONNECTION")) );
    CHECK( ( !headers.find_at_index("CONNECTION")) );
    CHECK( (headers.find("connection") == headers.end()) );
    CHECK( ( !!headers.at_key("COntent-length")) );
    CHECK( ( !!headers.find_at_index("COntent-length")) );
    CHECK( !!(headers.find("content-length") != headers.end()) );

    headers.remove_at_key("content-length");
    CHECK(headers.size() == 0);
    CHECK( ( !headers.at_key("COntent-length")) );
    CHECK( ( !headers.find_at_index("COntent-length")) );
    CHECK( !!(headers.find("content-length") == headers.end()) );

}
TEST_CASE("headers_same")
{
    HeaderFields h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeaderFields h1_dup{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeaderFields h1_reversed{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"44","4444444"},
        {"33","3333333"},
        {"22","2222222"},
        {"11","1111111"},
        {"ccc", "CCCCCC"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
    }};

    CHECK(h1.same_values(h1_dup));
    CHECK(h1.same_values(h1_reversed));

    CHECK(h1.same_order_and_values(h1_dup));
    CHECK( !h1.same_order_and_values(h1_reversed));
}

TEST_CASE("header_iskeep_alive")
{
    HeaderFields h1{{
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"Connection", "Keep-AliVe, another1, another2"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    CHECK(is_connection_keep_alive(std::string(" this has keep-alive embedded")));
    CHECK(is_connection_keep_alive(std::string(" this has Keep-alive embedded")));
    CHECK(is_connection_keep_alive(std::string(" this, has, Keep-Alive, embedded")));
    CHECK(is_connection_keep_alive(std::string(" this, has, KEEP-ALIVE, embedded")));
    CHECK(!is_connection_keep_alive(std::string(" this, has, Close, embedded")));
    CHECK(is_connection_keep_alive(std::string(" this has ,keep-alive, embedded")));
    CHECK(is_connection_keep_alive(std::string(" this has,Keep-alive,embedded")));
    CHECK(is_connection_keep_alive(std::string(" this, has, Keep-Alive , embedded")));
    CHECK(is_connection_keep_alive(std::string(" this, has, KEEP-ALIVE     embedded")));
    CHECK(!is_connection_keep_alive(std::string(" this, has, keep-aliveembedded")));
    CHECK(!is_connection_keep_alive(std::string(" this, haskeepalive, embedded")));
    CHECK(is_connection_keep_alive(std::string(" this, has, keep-alive;, embedded")));
    CHECK(is_connection_keep_alive(std::string("keep-alive;, embedded")));
    CHECK(is_connection_keep_alive(std::string("thi keep-alive")));
    CHECK(is_connection_keep_alive(std::string("keep-alive")));

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

    CHECK(is_connection_close(std::string(" this has close embedded")));
    CHECK(is_connection_close(std::string(" this has close embedded")));
    CHECK(is_connection_close(std::string(" this, has, cloSe, embedded")));
    CHECK(is_connection_close(std::string(" this, has, CLOSE, embedded")));
    CHECK(!is_connection_close(std::string(" this, has, keep-alive, embedded")));
    CHECK(is_connection_close(std::string(" this has ,close, embedded")));
    CHECK(is_connection_close(std::string(" this has,Close,embedded")));
    CHECK(is_connection_close(std::string(" this, has, Close , embedded")));
    CHECK(is_connection_close(std::string(" this, has, CLOSE     embedded")));
    CHECK(!is_connection_close(std::string(" this, has, closeembedded")));
    CHECK(!is_connection_close(std::string(" this, hasclose, embedded")));
    CHECK(is_connection_close(std::string(" this, has, close;, embedded")));
    CHECK(is_connection_close(std::string("close;, embedded")));
    CHECK(is_connection_close(std::string("thi close")));
    CHECK(is_connection_close(std::string("close")));

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
    HeaderFields h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    HeaderFields h2{h1};
    std::string bb = "bb";
    bool x = !!h2.at_key(bb);
    bool y = !!h1.at_key(bb);
    CHECK(x);
    CHECK(y);
    CHECK((h1.size() == h2.size()));
    HeaderFields h3{};
    h3 = h1;
    std::string ccc = "ccc";
    CHECK((h3.size() == h1.size()));
    CHECK(!!h1.at_key(ccc));
    CHECK(!!h3.at_key(ccc));
}
TEST_CASE("move test")
{
    HeaderFields h1{{
        {"Connection", "Keep-Alive, another1, another2"},
        {"bb","BBBBB"},
        {"ccc", "CCCCCC"},
        {"11","1111111"},
        {"22","2222222"},
        {"33","3333333"},
        {"44","4444444"}
    }};
    std::size_t h1_initial_size = h1.size();
    HeaderFields h2{std::move(h1)};
    std::string bb = "bb";
    bool x = !!h2.at_key(bb);
    bool y = !!h1.at_key(bb);

    CHECK(x);
    CHECK(!y);
    CHECK((h1.size() == 0));
    CHECK((h2.size() == h1_initial_size));
    HeaderFields h3{};
    h3 = std::move(h2);
    std::string ccc = "ccc";
    CHECK(!h1.at_key(ccc));
    CHECK(!h2.at_key(ccc));
    CHECK((h2.size() == 0));
    CHECK((h3.size() == h1_initial_size));
    CHECK(!!h3.at_key(ccc));
}
TEST_CASE("field efficient copy")
{
    std::string k = "1234567890123456789";
    std::string v = "this is a long value";
    auto k_p = (void*)k.c_str();
    auto v_p = (void*)v.c_str();
//    std::vector<Marvin::HeaderFields::Field> fields;
//    Marvin::HeaderFields::Field f(std::move(k),std::move(v));

    Marvin::HeaderFields::Field f(&(k), &(v));

    auto key_p = (void*)f.key.c_str();
    auto value_p = (void*)f.value.c_str();
    CHECK(key_p == k_p);
    CHECK(value_p == v_p);
    std::cout << __func__ << std::endl;
}