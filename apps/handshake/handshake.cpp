#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL TROG_LEVEL_WARN
#include <marvin/configure_trog.hpp>


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <cert/cert_store.hpp>
#include <marvin/certificates/certificates.hpp>
#include <marvin/certificates/env_utils.hpp>
#include "check_host.cpp"
#include "contact_server.hpp"

namespace po = boost::program_options;
namespace bf = boost::filesystem;


int main(int argc, const char * argv[])
{ 

    int opt;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("marvin", "use marvin handshaker")
        ("optimization", po::value<int>(&opt)->default_value(10), "optimization level")
        ("include-path,I", po::value< std::vector<std::string> >(),  "include path")
        ("input-file", po::value< std::vector<std::string> >(), "input file")
    // ;
    //     ("help", "produce help message")
        ("host", po::value<std::string>(), "specify or host or server to handshake with eg google.com")
        ("file", po::value<std::string>(), "specify a file that contains a list of hosts/servers to handshake with")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    bool marvin_flag = (vm.count("marvin"));
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("host") && vm.count("file")) {
        std::cout << "cannot specify both server and file options" << std::endl;
    } else if (vm.count("host")) {
        std::cout << "handshake with host : " << vm["host"].as<std::string>() << std::endl;
        std::string host_name = vm["host"].as<std::string>();
        ContactServer cs(marvin_flag);
        cs.handleHostName(1, host_name);
    } else if (vm.count("file")) { 
        std::cout << "handshake with host listed in  : "  << vm["file"].as<std::string>() << std::endl;
        std::string file = vm["file"].as<std::string>();
        ContactServer cs(marvin_flag);
        cs.handleFile(file);
    } else {
        std::cout << "Compression level was not set" << std::endl;
    }
}
