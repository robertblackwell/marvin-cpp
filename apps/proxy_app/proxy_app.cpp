
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <boost/asio.hpp>
#include <thread>
#include <regex>
#include <boost/filesystem.hpp>
#include <CLI/CLI.hpp>
#include <marvin/certificates/certificates.hpp>
#include <marvin/collector//capture_filter.hpp>
#include <marvin//collector//capture_collector.hpp>
#include "ctl_thread.hpp"
#include "mitm_thread.hpp"
#include "timer.hpp"

namespace bf = boost::filesystem;
using namespace Marvin;

int main(int argc, const char * argv[])
{ 

    long proxy_port;
    long ctl_port;

    std::string marvin_home;
    std::vector<std::string> https_mitm_regexes;
    std::vector<long> https_mitm_ports;
    std::string config_file;
    CLI::App app("Simple man in the middle proxy");
    app.set_help_all_flag("--help-all", "Extended help");
    app.add_option("--proxy-port", proxy_port, "port this proxy listens on. Always local host - default 9992");
    app.add_option("--marvin-home", marvin_home, "overrides MARVIN_HOME env variable");
    app.add_option("--https-mitm-regexes", https_mitm_regexes, 
        "regular expression that selects https connect request thats will be mitm'd");
    app.add_option("--https-mitm-ports", https_mitm_ports, 
        "ports that will be considered for mitm processing of https requests. Defaults to 447 ");
    app.add_option("--config-file", config_file, "config file path - not implemented");
    app.add_option("--ctl-port", ctl_port, "port for the http control interface default 9993");

    CLI11_PARSE(app, argc, argv);


    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    ERR_load_BIO_strings();
    ERR_load_ERR_strings();

    if(!app.count("--proxy-port")) {
        proxy_port = 9992;
    }
    auto x = app.count("--marvin-home");
    if(!app.count("--marvin-home")) {
        std::cout << "Error: a value is mandatory for the option --marvin-home. "  << std::endl;
        std::cout << "The value allows simple_mitm to find a store of trusted root certificates. "  << std::endl;
        std::cout << "Without these https traffic cannot be processed. "  << std::endl;
        exit(1);
    }
    Marvin::Certificates::init(marvin_home);

    boost::optional<long> proxy_port_opt;
    boost::optional<std::vector<long>> https_ports_opt;
    boost::optional<std::vector<std::string>> https_regexes_opt;

    if(app.count("--proxy-port")) {
        proxy_port_opt = (proxy_port);
    } else {
        proxy_port_opt = boost::none;
    }
    if(app.count("--https-mitm-ports")) {
        https_ports_opt = (https_mitm_ports);
    } else {
        https_ports_opt = boost::none;
    }

    CaptureFilter::SPtr capture_filter_sptr;

    if(app.count("--https-mitm-regexes")) {
        https_regexes_opt = (https_mitm_regexes);
        capture_filter_sptr = std::make_shared<CaptureFilter>(https_mitm_regexes);
    } else {
        https_regexes_opt = boost::none;
        std::vector<std::string> dummy{".*"};
        capture_filter_sptr = std::make_shared<CaptureFilter>(dummy);
    }

    auto xx =  app.count("--ctl-port");
    if(!app.count("--ctl-port")) {
        ctl_port = 9993;
    }

    std::cout << "Simple proxy starting: " << std::endl;
    std::cout << "\tMitm listening on port: " << proxy_port << std::endl;
    std::cout << "\tControl on port : " << ctl_port << std::endl;
    std::cout << "\tMarvin home : " << marvin_home << std::endl;

    std::cout << "Simple proxy starting - listen on port" << proxy_port << std::endl;

    boost::asio::io_context io_ctl;
    CaptureCollector::SPtr collector_sptr = std::make_shared<CaptureCollector>(io_ctl, capture_filter_sptr);

    Marvin::MitmThread mitm_thread(
        proxy_port,
        marvin_home,
        capture_filter_sptr,
        collector_sptr
    );
    mitm_thread.start();
    MyTimer  timer(io_ctl, 2);
    timer.arm([collector_sptr]()
    {
        std::cout << "Timer call back from proxy_app" << std::endl << std::flush;
        auto traffic = collector_sptr->get_captures();
        if (traffic.size() > 0)
            std::cout << "Size of captured host list first host: " << traffic.front()->host << " size: " << traffic.front()->exchanges.size() << std::endl << std::flush;
        else
            std::cout << "No captured traffic" << std::endl;
    });
    io_ctl.run();
    mitm_thread.join();

}
