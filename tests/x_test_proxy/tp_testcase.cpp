
#include "tp_testcase.hpp"
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

using namespace tp;

Testcase::Testcase(Marvin::Http::MessageBaseSPtr msgSPtr, std::string scheme, std::string proxyHost, std::string proxyPort)
:m_msg_sptr(msgSPtr), m_proxy_scheme(scheme), m_proxy_host(proxyHost), m_proxy_port(proxyPort)
{
}

