#ifndef marvin_certificate_hpp
#define marvin_certificate_hpp
#include <memory>                       // for shared_ptr
#include <cert/cert_authority.hpp>      // for AuthoritySPtr
#include <cert/cert_certificate.hpp>    // for Certificate
#include <cert/cert_identity.hpp>       // for Identity
#include <cert/cert_store_locator.hpp>  // for LocatorSPtr
#include <cert/cert_store_store.hpp>    // for StoreSPtr
#include <openssl/ossl_typ.h>           // for X509_STORE

namespace Cert { class Builder; }

namespace Marvin {
class Certificates;  // lines 12-12
typedef  std::shared_ptr<Certificates> CertificatesSPtr;
class Certificates
{
public:
	static Certificates& getInstance();
	Certificates();
	
	X509_STORE* getX509StorePtr();

	Cert::Identity buildServerMitmCertificate(Cert::Certificate original_cert);

private:
	X509_STORE*                     m_X509_store_ptr;
    std::shared_ptr<Cert::Builder>  m_builder_sptr;
    Cert::Store::StoreSPtr          m_cert_store_sptr;
    Cert::Store::LocatorSPtr        m_locator_sptr;
    Cert::AuthoritySPtr             m_authority_sptr;

};
} //namespace 
#endif
