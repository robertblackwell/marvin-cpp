#ifndef cert_x509_time_include_hpp
#define cert_x509_time_include_hpp

//
//https://zakird.com/2013/10/13/certificate-parsing-with-openssl
//http://fm4dd.com/openssl/add_oids_to_openssl.htm
//http://www.zytrax.com/tech/survival/ssl.html
//https://apidock.com/ruby/v1_9_3_125/OpenSSL/X509/Certificate/extensions%3D
//http://openssl.cs.utah.edu/docs/apps/x509v3_config.html#certificate_policies_
//https://superuser.com/questions/738612/openssl-ca-keyusage-extension

#include "x509.hpp"

namespace Cert {
namespace x509 {
#pragma mark - time  functions

    std::string TimeAsString(ASN1_TIME* tm);

} // namespace x509
} //namespace Cert

#endif
