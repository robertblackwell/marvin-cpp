/// \brief this file (and its ccp) implement tests aim at verifying the correct
/// operation of the man-in-the-middle proxy.
///
/// These tests send correctly formated
/// proxy request to the mitm proxy, these should be forwarded on our test server
/// verified and returned where they are again verified.
///
///
#ifndef marvin_proxy_fixture_tests_hpp
#define marvin_proxy_fixture_tests_hpp
#include <marvin/http/message.hpp>
#include "tp_testcase.hpp"

class ProxyFixture {
public:
    ProxyFixture();
    boost::filesystem::path expectedFilePathForTest(std::string test_name);
    boost::filesystem::path m_this_file;
    boost::filesystem::path m_file_dir;
    boost::filesystem::path m_received;
    boost::filesystem::path m_expected;
    boost::filesystem::path m_received_fixed;
    std::string m_collector_file_path;
    long m_proxy_port;
    std::string m_proxy_host;
    std::string m_proxy_scheme;

};

#endif