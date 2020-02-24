#ifndef x509_cert_builder_include_hpp
#define x509_cert_builder_include_hpp
//#include "constants.hpp"
//#include "version_check.hpp"
//#include "x509.hpp"
//#include "cert_store_authority.hpp"

namespace Cert {
namespace x509{
#pragma mark - builder static function

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
     * @return X509*    This object was allocated inside this function. It is the callers responsibility to manage the lifetime 
     *                  of the object/pointer. Free is with X509_free(x)
     */
    X509* create(
         X509*              ca_cert,
         EVP_PKEY*          ca_private_key,
         Cert::x509::SerialNumber   serial,
         Cert::x509::Version        version,
         EVP_PKEY*          new_pkey_pair,
         Cert::x509::TimeOffset     not_before_offset,
         Cert::x509::TimeOffset     not_after_offset,
         Cert::x509::NameSpecification subject_name_spec,
         std::string subject_alt_name_string,
         Cert::x509::ExtSpecifications extra_extensions
         );


}; // namespace x509
} // namespace Cert
#endif
