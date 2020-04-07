//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>
#include <marvin/helpers/mitm.hpp>

#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include <marvin/http/uri.hpp>

using namespace Marvin;
using namespace std;

#if 0

namespace NotVirtual {
bool base_destructor_called = false;
class base {
  public:
    base()
    { cout<<"Constructing base \n"; }
     ~base()    { cout<<"Destructing base \n"; }
};
  
class derived: public base {
  public:
    derived() {
        cout<<"Constructing derived \n";
    }
    ~derived(){
        cout<<"Destructing derived \n";
    }
};
}
namespace Virtual {
bool base_destructor_called = false;
class base {
  public:
    base() {
        cout<<"Constructing base \n";
    }
     virtual ~base() {
        cout<<"Destructing base \n";
        base_destructor_called = true;
     }
};
  
class derived: public base {
  public:
    derived(){
        cout<<"Constructing derived \n";
    }
    ~derived(){
        cout<<"Destructing derived \n";
    }
};
}
TEST_CASE("something")
{
    class m: public Marvin::MessageBase {
    public:
        ~m() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }
    };
    using namespace Marvin;
    MessageBase* msg = new MessageBase();
    delete msg;
    m* minst = new m();
    delete minst;
}
TEST_CASE("not virt dtor")
{
    using namespace NotVirtual;
        derived *d = new derived();
        base *b = d;
        delete b;
        REQUIRE(! base_destructor_called);
}
TEST_CASE("virt dtor")
{
    using namespace Virtual;
        derived *d = new derived();
        base *b = d;
        delete b;
        REQUIRE(base_destructor_called);
}
#endif

TEST_CASE("uri_http_no_missing_elements")
{
    Marvin::Uri uri(std::string("http://www.somewhere.com:243/path1/path2?one=111&two=222"));
    CHECK(uri.scheme() == "http");
    CHECK(uri.host() == "www.somewhere.com:243");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 243);
    CHECK(uri.relativePath() == "/path1/path2?one=111&two=222");
    std::cout << "uri" << std::endl;
}
TEST_CASE("uri_https_no_missing_elements")
{
    Marvin::Uri uri(std::string("https://www.somewhere.com:243/path1/path2?one=111&two=222"));
    CHECK(uri.scheme() == "https");
    CHECK(uri.host() == "www.somewhere.com:243");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 243);
    CHECK(uri.relativePath() == "/path1/path2?one=111&two=222");
    std::cout << "uri" << std::endl;
}

TEST_CASE("uri_https_port_missing")
{
    Marvin::Uri uri(std::string("https://www.somewhere.com/path1/path2?one=111&two=222"));
    CHECK(uri.scheme() == "https");
    CHECK(uri.host() == "www.somewhere.com:443");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 443);
    CHECK(uri.relativePath() == "/path1/path2?one=111&two=222");
    std::cout << "uri" << std::endl;
}
TEST_CASE("uri_http_port_missing")
{
    Marvin::Uri uri(std::string("http://www.somewhere.com/path1/path2?one=111&two=222"));
    CHECK(uri.scheme() == "http");
    CHECK(uri.host() == "www.somewhere.com:80");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 80);
    CHECK(uri.relativePath() == "/path1/path2?one=111&two=222");
    CHECK(uri.search() == "one=111&two=222");
    std::cout << "uri" << std::endl;
}
TEST_CASE("uri_http_missing_port_query")
{
    Marvin::Uri uri(std::string("http://www.somewhere.com/path1/path2"));
    CHECK(uri.scheme() == "http");
    CHECK(uri.host() == "www.somewhere.com:80");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 80);
    CHECK(uri.relativePath() == "/path1/path2");
    CHECK(uri.search() == "");
    std::cout << "uri" << std::endl;
}
TEST_CASE("uri_http_missing_port_path_query")
{
    Marvin::Uri uri(std::string("http://www.somewhere.com"));
    CHECK(uri.scheme() == "http");
    CHECK(uri.host() == "www.somewhere.com:80");
    CHECK(uri.server() == "www.somewhere.com");
    auto x = uri.port();
    CHECK(uri.port() == 80);
    CHECK(uri.relativePath() == "/");
    CHECK(uri.search() == "");
    std::cout << "uri" << std::endl;
}
