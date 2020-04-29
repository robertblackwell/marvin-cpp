//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <thread>
#include <regex>
#include <boost/process.hpp>

#include <marvin/collector/pipe_collector.hpp>
#include <marvin/collector/collector_base.hpp>
#include <marvin/server_v3/tcp_server.hpp>
#include <marvin/server_v3/mitm_app.hpp>

void start_mitm_thread()
{
    using namespace Marvin;

    std::vector<std::regex> re{std::regex("^ssllabs(.)*$")};
    std::vector<int> ports{443, 9443};
    MitmApp::configSet_HttpsPorts(ports);
    MitmApp::configSet_HttpsHosts(re);

    TcpServer* server_ptr;

    std::function<void(void*)> proxy_thread_func = [&server_ptr](void* param) {
        server_ptr = new Marvin::TcpServer([](boost::asio::io_service& io) {
            CollectorBaseSPtr cb_sptr = std::make_shared<CollectorBase>(io, std::cout);
            MitmAppUPtr app_uptr = std::make_unique<MitmApp>(io, cb_sptr);
            return app_uptr;
        });
        server_ptr->listen(9992);
        std::cout << "Returned from listen" << std::endl;
    };
    std::thread proxy_thread(proxy_thread_func, nullptr);

    proxy_thread.join();
    delete server_ptr;
}

