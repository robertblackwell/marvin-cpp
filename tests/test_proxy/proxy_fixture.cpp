#include <catch2/catch.hpp>
#include <boost/process.hpp>
#include <marvin/http/message_factory.hpp>
#include <marvin/helpers/helpers_fs.hpp>
#include "proxy_fixture.hpp"
using namespace Marvin;
using namespace Http;

ProxyFixture::ProxyFixture():
    m_this_file(boost::filesystem::path(__FILE__))

{
    m_file_dir = (m_this_file.parent_path().parent_path() / "test_proxy");
    m_file_dir = (m_this_file.parent_path());
    m_received = (m_file_dir / "proxy_received");
    m_expected = (m_file_dir / "proxy_expected");
    m_received_fixed = (m_file_dir / "proxy_received_fixed");
    m_collector_file_path = (m_received.string());
    m_proxy_port = 9992;
    m_proxy_host = "localhost";
    m_proxy_scheme = "http";
}
boost::filesystem::path ProxyFixture::expectedFilePathForTest(std::string test_name) {
    return (m_file_dir / (test_name + std::string("_expected")));
}

