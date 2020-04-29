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

#define MARVIN_DOT_MARVIN_FILE ".marvin"
#define MARVIN_CA_CONFIG_FILE "ca_config.json"
#define MARVIN_CERT_STORE "cert_store"


namespace Marvin {
void displayX509(Cert::Certificate cert);

// used mostly in getenv() calls
extern const char* kMarvinEnvKey_MarvinHome;
extern const std::string kMarvinDotDirectoryName;
extern const std::string kMarvinCaConfigFileName;
extern const std::string kMarvinCertStoreName; 

#undef MARVIN_CERTIFICATES_MEYERS_SINGLTON

class Certificates;  // lines 12-12
typedef  std::shared_ptr<Certificates> CertificatesSPtr;
class Certificates
{
public:
	
	#ifndef MARVIN_CERTIFICATES_MEYERS_SINGLTON

	static std::unique_ptr<Certificates> s_instance_uptr;
	static void init(boost::filesystem::path marvin_home);

	#endif
	static Certificates& getInstance();
	static ::Cert::Store::StoreSPtr createStore(
		boost::filesystem::path store_path, 
		boost::filesystem::path ca_config_file_path
	);
	Certificates();
	Certificates(boost::filesystem::path marvin_home);
	~Certificates();
	
	X509_STORE* getX509StorePtr();

	Cert::Identity buildServerMitmCertificate(std::string host, Cert::Certificate original_cert);

private:
	X509_STORE*                     m_X509_store_ptr;
    std::shared_ptr<Cert::Builder>  m_builder_sptr;
    Cert::Store::StoreSPtr          m_cert_store_sptr;
    Cert::Store::LocatorSPtr        m_locator_sptr;
    Cert::AuthoritySPtr             m_authority_sptr;

};
} //namespace 
#endif
