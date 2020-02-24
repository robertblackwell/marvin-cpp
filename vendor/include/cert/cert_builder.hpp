/**
 * header file for Cert functions that need to know about crypto types - which I hope will be internal
 * implementation details
 */
#ifndef certlib_oo_cert_builder_hpp
#define certlib_oo_cert_builder_hpp
#include <map>
#include "x509.hpp"
#include "cert_identity.hpp"
/**
* @brief Cert is the top level namespace for libcert
*/
namespace Cert {
class Certificate;

/**
* @brief Cert::Builder is a namespace that wraps the functions that build x509 certificates
*/
class Builder {

private:
    // used to cache results of Mitm build for future use
    struct MitmResult {
        std::string host_id;
        std::string pem_original_certificate;
        Identity identity;
    };

public:
    
    Builder(Authority& certAuth);
    ~Builder();
    
    /**
     * \brief This function is the reason d'etre for this project.
     *
     * Build a new certificate - all the required and optional ingredients
     * are provided for in the arguments. This ensures that the input data is complete
     * when the signing action takes place.
     *
     * Note - building a certificate requies a certificate authorities certificate and private key
     *
     * @param ca_cert           X509*                   the certificate of the Certificate Authority that will sign this certificate
     *
     * @param ca_private_key    EVP_PKEY*               the private key (EVP_PKEY*) of the signing certificate authprity
     *
     * @param serial            Cert::x509::SerialNumber   the serial number that will be put into the certificate
     *
     * @param version           Cert::x509::Version        the certificate version number, value of Cert::x509::Version
     *
     * @param new_pkey_pair     EVP_PKEY*               generated from EVP_PKEY* new_pkey = x509Rsa_Generate();
     *                                                  we only use RSA keys in this implementation - convenience only
     *
     * @param not_before_offset Cert::x509::TimeOffset          the value in seconds as an offset from NOW before which this certificate in NOT valid
     *
     * @param not_after_offset  Cert::x509::TimeOffset          the value in seconds as an offset from NOW after which this certificate in NOT valid
     *
     * @param subject_name_spec Cert::x509::NameSpecification   specifies the components of the subject name,
     *
     * @param subject_alt_name_string                   std::string a comma separated list of alternative subject names, in the form
     *                                                  "DNS:firstname.com, DNS:secondname.com"
     *
     * @param extra_extensions  x509ExtSpecification    specifies the extensions to be added to the certificate.
     *                                                  a map indexed by int  - value is a NID #define value (see openssl/objects.h)
     *                                                  the value is a std:string - but actually its a specification rather than a value
     *                                                  see, for example http://openssl.cs.utah.edu/docs/apps/x509v3_config.html#certificate_policies_
     *                                                  for more details
     *
     * @return Cert::Certificate
     */
    ::Cert::Certificate build(
         Cert::x509::SerialNumber       serial,
         Cert::x509::Version            version,
         EVP_PKEY*                      new_pkey_pair,
         Cert::x509::TimeOffset         not_before_offset,
         Cert::x509::TimeOffset         not_after_offset,
         Cert::x509::NameSpecification  subject_name_spec,
         std::string                    subject_alt_name_string,
         Cert::x509::ExtSpecifications  extra_extensions
         );

    /**
     * Create a new certificate and private key based on an original certificate and
     * signed by the given Certificate Authority. The result is suitable for a MITM proxy
     * to use to intercept SSL/TLS traffic (with the cooperation of the client side of course.
     *
     * @param certAuth CertificateAuthority - the certificate authority to sign the new certificate
     *
     * @return Cert::Identity
     *
     * The Builder class has the capacity to "memorize" (or cache) results.
     */
    ::Cert::Identity buildMitmIdentity(Cert::Certificate& original_cert);
    
    private:
        Cert::Authority& m_cert_auth;
        std::map<std::string, MitmResult> m_cache;


}; // class/namespace Builder
} // namespace Cert
#endif
