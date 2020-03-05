#ifndef marvin_certificate_hpp
#define marvin_certificate_hpp
#include <boost/filesystem.hpp>
#include <openssl/x509.h>
#include <cert/cert_authority.hpp>
#include <cert/cert_certificate.hpp>
#include <cert/cert_identity.hpp>
#include <cert/cert_store.hpp>

#include <cert/cert_builder.hpp>

class Certificates;
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

#endif
