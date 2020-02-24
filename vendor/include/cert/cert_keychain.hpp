#ifndef certlib_cert_store_keychain_include_hpp
#define certlib_cert_store_keychain_include_hpp
namespace Cert{

/**
 * \brief Provides a namespace for functions that work with the OSX keychain to import the root certificate bundle
 */
class Keychain
{
    public:

	/// https://stackoverflow.com/questions/35031149/set-imported-certificate-to-always-be-trusted-in-mac-os-x
	/// sudo security add-trusted-cert -d -r trustRoot -p [option] -k /Library/Keychains/System.keychain <certificate>
	/// -p options are ssl, smime, codeSign, IPSec, iChat, basic, swUpdate, pkgSign, pkinitClient, pkinitServer, timestamping, eap
	
	/**
	* importCAIdentity - adds the CA certificate from the subject store
	* to the OSX keychain
	* @param store - the certificate store in question
	* @param keychain - not used
	* @throws if something goes wrong
	*/
	static void importCAIdentity(::Cert::Store::Store& store, std::string keyChain);

	/**
	* getRooCerts - exports the store of root certificates
	* from the OSX keychain and appends the CA root certificate from the subject cert store
	* @param store - the certificate store in question
	* @throws if something goes wrong
	*/
	static void importRootCerts(::Cert::Store::Store& store, std::string keyChain);

    #if 0
	/**
	* KeyChain::verifyInterceptorCertificate - verifies that the interceptor certificate will verify 
	* against the osx root file.
	*
	* @param $store - the certificate store in question
	* @param $host - name of the subject host
	* @return nothing
	* @throws if something goes wrong
	*
	*/
	public static function verifyInterceptorCert(CertStore $store, $host)
	{
		$roots_file = $store->extended_osx_root_certs;

		$server_cert = $store->host_certificate_path($host);

		\CertUte\CertStore\Helpers::exec(
			"openssl verify 	-verbose -x509_strict -CAfile $roots_file -CApath nosuchpath $server_cert"
		);

	}
    #endif
};
} //namespace Cert
#endif
