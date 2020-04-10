
#include <marvin/certificates/certificates.hpp>
#include <openssl/x509.h>                   // for X509_STORE_load_locations
#include <cstdlib>                          // for getenv, NULL
#include <iosfwd>                           // for string
#include <string>                           // for basic_string
#include <boost/filesystem/operations.hpp>  // for is_directory
#include <boost/filesystem/path.hpp>        // for path
#include <boost/format.hpp>
#include <cert/cert_authority.hpp>          // for AuthoritySPtr
#include <cert/cert_builder.hpp>            // for Builder
#include <cert/cert_certificate.hpp>        // for Certificate
#include <cert/cert_identity.hpp>           // for Identity
#include <cert/cert_store_types.hpp>        // for Path
#include <cert/error.hpp>                   // for THROW
#include <cert/cert_store_locator.hpp>      // for Locator, LocatorSPtr
#include <cert/cert_store_store.hpp>        // for Store, StoreSPtr
#include <openssl/ossl_typ.h>               // for X509_STORE
#include <marvin/helpers/macros.hpp>
#include <marvin/certificates/env_utils.hpp>

namespace Marvin {

const char* kMarvinEnvKey_MarvinHome = "MARVIN_HOME";
const std::string kMarvinDotDirectoryName = ".marvin";
const std::string kMarvinCaConfigFileName = "ca_config.json";
const std::string kMarvinCertStoreName = "cert_store"; 


Certificates& Certificates::getInstance()
{
    static Certificates instance{};
    return instance;
}
::Cert::Store::StoreSPtr Certificates::createStore(boost::filesystem::path store_path, boost::filesystem::path ca_config_file_path)
{
    boost::filesystem::path config_path = ca_config_file_path;
    if (! boost::filesystem::exists(config_path)) {
        THROW("ca_config_file does not exist at given path " + config_path.string());
    }
    boost::filesystem::path where = store_path;
    if (boost::filesystem::exists(where)) {
        boost::filesystem::remove_all(where);
    }
    Cert::Store::StoreSPtr store_sptr = Cert::Store::Store::makeEmpty(where);
    store_sptr->m_cert_auth_sptr = Cert::Authority::create(store_sptr->m_locator_sptr->ca_dir_path, config_path);
    store_sptr->saveConfig(config_path.string());
    store_sptr->loadConfig();
    store_sptr->loadCertAuth();
    return store_sptr;
}
Certificates::Certificates()
{
    using namespace boost;
    using namespace boost::filesystem;

    optional<path> mh = Marvin::getEnvMarvinHome();
    path cwd = boost::filesystem::current_path();
    if (!mh) {
        THROW("environment variable MARVIN_HOME not set");
    } else if (!Marvin::validEnvVariables()) {
        THROW(str(boost::format("Marvin environment variables MARVIN_HOME=%1% invalid") % mh.get().string()));
    } else if (!Marvin::validWorkingDir()) {
        THROW(str(boost::format("running Certificate::getInstance() from invalid working directory cwd = %1%") % cwd.string()));
    } 
    path p = mh.get() / Marvin::kMarvinDotDirectoryName / Marvin::kMarvinCertStoreName;
    path base(p);
    if (! boost::filesystem::is_directory(base)) {
        THROW("the base dir for certificates does not exist");
    }
    m_cert_store_sptr   = Cert::Store::Store::load(p);
    m_locator_sptr      = m_cert_store_sptr->getLocator();
    m_authority_sptr    = m_cert_store_sptr->getAuthority();
    m_builder_sptr      = std::make_shared<Cert::Builder>(*m_authority_sptr);
    auto bundle_path = m_locator_sptr->mozilla_root_certs.string();
    m_X509_store_ptr = X509_STORE_new();
    X509_STORE_load_locations(m_X509_store_ptr, (const char*)bundle_path.c_str(), NULL);
    X509_STORE_up_ref(m_X509_store_ptr);
}
X509_STORE* Certificates::getX509StorePtr()
{
    X509_STORE_up_ref(m_X509_store_ptr);
    return m_X509_store_ptr;
}

Cert::Identity Certificates::buildServerMitmCertificate(Cert::Certificate original_certificate)
{
    Cert::Identity tmp = m_builder_sptr->buildMitmIdentity(original_certificate);
    return tmp;
}
} // namespace Marvin
