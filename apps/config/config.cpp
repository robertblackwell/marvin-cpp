//
// The main entry point for Marvin - a mitm proxy for http/https 
//


#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include <boost/filesystem.hpp>

#include <cert/cert_store.hpp>
#include <marvin/certificates/certificates.hpp>
#include <marvin/certificates/env_utils.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_GLOBAL_LEVEL(Trog::LogLevelVerbose)
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)


int main(int argc, const char * argv[])
{
    using namespace boost::filesystem;

    std::cout 
        << std::endl
        << "Welcome This utility builds a certificate authority and a certificate bundle" << std::endl
        << "for use by the Marvin Mitm Proxy" << std::endl
        << std::endl;

    auto ev1 = getenv("HOME");
    auto ev2 = getenv(Marvin::kMarvinEnvKey_MarvinHome);
    if (!Marvin::validWorkingDir()) {
        THROW("invalid working directory");
    }
    if (!Marvin::validEnvVariables()) {
        THROW("invalid MARVIN environment variables ");
    }
    path marvin_home = Marvin::getEnvMarvinHome().get();
    path marvin_dot_dir = marvin_home / Marvin::kMarvinDotDirectoryName;

    path marvin_ca_config_file = marvin_dot_dir / Marvin::kMarvinCaConfigFileName;
    path marvin_cert_store = marvin_dot_dir / Marvin::kMarvinCertStoreName;
    Cert::Store::LocatorSPtr locator_sptr = std::make_shared<Cert::Store::Locator>(marvin_cert_store);

    if (!is_directory(marvin_home)) {
        THROW("marvin home directory does not exist or is not a directory");
    }
    if (!is_directory(marvin_dot_dir)) {
        THROW("the .marvin directory does not exist or is not a directory");
    }
    if (!is_regular_file(marvin_ca_config_file)) {
        THROW("the file '.marvin/ca_config.json' file does not exist");
    }
    if (is_directory(marvin_cert_store)) {
        std::cerr 
            << "the certificate store directory :" 
            << marvin_cert_store 
            << " already exists." << std::endl
            << std::endl << "It will be overwritten if you continue " << std::endl
            << std::endl << "Type yes and hit the return key to continue ctrl-c to exist" << std::endl
            << std::endl;
        std::string instr;
        std::cin >> instr;
        std::cout << "Thanks you: " << instr << std::endl;
    }
    path where = marvin_cert_store;
    if (boost::filesystem::exists(where)) {
        boost::filesystem::remove_all(where);
    }
    Cert::Store::StoreSPtr store_sptr = Cert::Store::Store::makeEmpty(where);
    store_sptr->m_cert_auth_sptr = Cert::Authority::create(store_sptr->m_locator_sptr->ca_dir_path, marvin_ca_config_file);
    store_sptr->rootCertsFromMozilla();
    store_sptr->rootCertsMozillaActive();

    store_sptr->saveConfig(marvin_ca_config_file.string());
    store_sptr->loadConfig();
    store_sptr->loadCertAuth();

    // Cert::Store::StoreSPtr store_sptr = Cert::Store::Store::makeWithCA(where, config_path);
    std::cout << "marvin config utility - builds a certificate authority" << std::endl;
    
    return 0;
}
