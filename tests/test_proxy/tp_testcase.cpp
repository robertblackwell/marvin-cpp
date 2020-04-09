
#include "tp_testcase.hpp"
Trog_SETLEVEL(LOG_LEVEL_WARN)

using namespace tp;

Testcase::Testcase(Marvin::Http::MessageBaseSPtr msgSPtr, std::string scheme, std::string proxyHost, std::string proxyPort)
:m_msg_sptr(msgSPtr), m_proxy_scheme(scheme), m_proxy_host(proxyHost), m_proxy_port(proxyPort)
{
}

