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
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include "boost_stuff.hpp"
#include "http_header.hpp"
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)
namespace Http {
    class HeadersIter;

    class Headers {
        public:
            Headers();
            ~Headers();
            void set(std::string k, std::string v);
            HeadersIter find(std::string k);
            void remove(std::string k);
            bool has(std::string k);
            std::size_t size();
            HeadersIter Begin();
            HeadersIter End();
            friend class HeadersIter;
        protected:
            std::map<std::string, int>  m_keys;
            std::vector<std::string>    m_values;
    };
    HeadersIter Headers::Begin()
    {
    
    }
    HeadersIter Headers:end()
    {
    
    }
    class HeadersIter {
        public:
        HeadersIter(Headers& headers, int size);
        HeadersIter operator++(int);
        protected:
            Headers& m_headers;
            int      m_size;
    };
    
    HeadersIter::HeadersIter(Headers& headers, int size): m_headers(headers), m_size(size)
    {
        
    }
    HeadersIter HeadersIter::operator++(int i)
    {
    
    }
Headers::Headers() : m_keys(std::map<std::string, int>()), m_values(std::vector<std::string>())
{
}
void Headers::set(std::string k, std::string v)
{
    assert(m_values.size() == m_keys.size());
    int next_index = (int)m_values.size();
    if( m_keys.find(k) == m_keys.end()){
        m_values.push_back(v);
        m_keys[k] = next_index;
    } else {
    
    }
}
TEST_CASE("headers01", "01")
{
}

TEST_CASE("header02","02")
{

}

int main( int argc, char* argv[] )
{
    // global setup - run a server
    RBLogging::setEnabled(false);
    char* _argv[2] = {argv[0], (char*)"-r tap"}; // change the filter to restrict the tests that are executed
    int _argc = 2;
    printf("headers\n");
    int result = Catch::Session().run( argc, argv );
    printf("headers\n");
    return result;
}

