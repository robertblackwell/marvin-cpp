
#include <marvin/certificates/certificates.hpp>
#include <openssl/x509.h>                   // for X509_STORE_load_locations
#include <cstdlib>                          // for getenv, NULL
#include <iosfwd>                           // for string
#include <string>                           // for basic_string
#include <boost/filesystem/operations.hpp>  // for is_directory
#include <boost/filesystem/path.hpp>        // for path
#include <cert/cert_authority.hpp>          // for AuthoritySPtr
#include <cert/cert_builder.hpp>            // for Builder
#include <cert/cert_certificate.hpp>        // for Certificate
#include <cert/cert_identity.hpp>           // for Identity
#include <cert/cert_store_types.hpp>        // for Path
#include <cert/error.hpp>                   // for THROW
#include <cert/cert_store_locator.hpp>      // for Locator, LocatorSPtr
#include <cert/cert_store_store.hpp>        // for Store, StoreSPtr
#include <openssl/ossl_typ.h>               // for X509_STORE



Certificates& Certificates::getInstance()
{
    static Certificates instance{};
    return instance;
}
Certificates::Certificates()
{
    auto x = std::getenv("MARVIN_CERT_STORE_PATH");
    if (std::getenv("MARVIN_CERT_STORE_PATH") == nullptr) {
        
        THROW("no environment variable MARVIN_CERT_STORE_PATH");
    }
    std::string p = std::getenv("MARVIN_CERT_STORE_PATH");
    boost::filesystem::path base(p);
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
    return m_X509_store_ptr;
}

Cert::Identity Certificates::buildServerMitmCertificate(Cert::Certificate original_certificate)
{
    Cert::Identity tmp = m_builder_sptr->buildMitmIdentity(original_certificate);
    return tmp;
}

