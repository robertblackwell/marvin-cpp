
#include <string>
#include <regex>
#include "runners.hpp"
#include "server_runner.hpp"
#include "tp_proxy_runner.hpp"

static std::shared_ptr<ServerRunner> serverRunnerSPtr;
static std::shared_ptr<ProxyRunner> proxyRunnerSPtr;

void startTestServer(long port)
{
    serverRunnerSPtr = std::make_shared<ServerRunner>();
    serverRunnerSPtr->setup(port);
}
void startProxyServer(long port)
{
    PipeCollector::configSet_PipePath("/Users/rob/marvin_collect");

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    ForwardingHandler::configSet_HttpsPorts(ports);
    ForwardingHandler::configSet_HttpsHosts(re);
    proxyRunnerSPtr = std::make_shared<ProxyRunner>();
    proxyRunnerSPtr->setup(port);
}
void stopTestServer()
{
    serverRunnerSPtr->teardown();
}
void stopProxyServer()
{
    proxyRunnerSPtr->teardown();
}
