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
#include <marvin/forwarding//forward_helpers.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

#include <marvin/http/uri.hpp>

using namespace Marvin;
using namespace Marvin::Http;
using namespace std;


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
    class m: public Marvin::Http::MessageBase {
    public:
        ~m() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }
    };
    using namespace Marvin::Http;
    MessageBase* msg = new MessageBase();
    delete msg;
    m* minst = new m();
    delete minst;
}
TEST_CASE("uri")
{
    Marvin::Uri uri(std::string("http://www.somewhere.com:443?one=111&two=222"));
    std::cout << "uri" << std::endl;
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
