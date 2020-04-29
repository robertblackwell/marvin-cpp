//
// The main entry point for Marvin - a mitm proxy for http/https 
//


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
#include <boost/program_options.hpp>
#include "ctl_thread.hpp"
#include "mitm_thread.hpp"

namespace po = boost::program_options;
namespace bf = boost::filesystem;


int main(int argc, const char * argv[])
{ 

    int opt;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("proxy-port", po::value<long>(), "port this proxiy listens on. Always local host - default 9992")
        ("marvin-home", po::value<std::string>(), "overrides MARVIN_HOME env variable")
        ("https-mitm-regex", po::value<std::vector<std::string>>(), "regular expression that selects https connect request thats will be mitm'd")
        ("https-mitm-ports", po::value<std::vector<std::string>>(), "ports that will be considered for mitm processing of https requests. Defaults to 447 ")

        ("config-file,C", po::value<std::string>(),  "config file path - not implemented")
        ("ctl-pipe-path,P", po::value<std::string>(), "path name for the control interface named pipe. If none no control interface")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    bool marvin_flag = (vm.count("marvin"));
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    boost::optional<long> proxy_port;
    if(vm.count("proxy-port")) {
        proxy_port = vm["proxy-port"].as<long>();
    } else {
        proxy_port = boost::none;
    }

    boost::optional<std::string> marvin_home;
    if(vm.count("marvin-home")) {
        marvin_home = vm["marvin-home"].as<std::string>();
    } else {
        marvin_home = boost::none;
        std::cout << "Error: a value is mandatory for the option --marvin-home. "  << std::endl;
        std::cout << "The value allows simple_mitm to find a store of trusted root certificates. "  << std::endl;
        std::cout << "Without these https traffic cannot be processed. "  << std::endl;
        exit(1);
    }

    boost::optional<std::vector<std::string>> https_regex;
    if(vm.count("https-regex")) {
        https_regex = vm["https-regex"].as<std::vector<std::string>>();
    } else {
        https_regex = boost::none;
    }

    boost::optional<std::vector<std::string>> https_ports;
    if(vm.count("https-ports")) {
        https_ports = vm["https-ports"].as<std::vector<std::string>>();
        } else {
        https_ports = boost::none;
    }

    Marvin::MitmThread mitm_thread(
        proxy_port,
        marvin_home,
        https_regex,
        https_ports
    );

    boost::optional<std::string> ctl_path;
    if(vm.count("ctl-pipe-path")) {
        ctl_path = vm["ctl-pipe-path"].as<std::string>();
    } else {
        ctl_path = boost::none;
    }

    Marvin::CtlThread ctl_thread(ctl_path);

    ctl_thread.getThread().join();
    mitm_thread.getThread().join();

}
