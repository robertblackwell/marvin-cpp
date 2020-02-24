#ifndef cert_x509_req_include_hpp
#define cert_x509_req_include_hpp
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>


#pragma mark x509Req functions

namespace Cert {
namespace x509 {

    X509_REQ* x509Req_New();

    void x508Req_AddExtensions(X509_REQ* req, STACK_OF(X509_EXTENSION)* stack);

    void x509Req_SetPublicKey(X509_REQ* req, EVP_PKEY* pkey );
    void x509Req_SetSubjectName(X509_REQ* req, std::map<std::string, std::string> subjectValues);

    void x509Req_VerifySignature(X509_REQ* req);
    void x509Req_Sign(X509_REQ* req, EVP_PKEY* pkey, EVP_MD* digest);

    X509_REQ* x509Req_ReadFromFile(std::string fileName);
    void x509Req_WriteToFile(X509_REQ* req, std::string filename);

    X509_NAME* x509Req_GetSubjectName(X509_REQ* req);

    X509_EXTENSION* x509Req_GetSubjectAltName(X509_REQ* req);

    void x509Req_PrintNames(BIO* out, X509_REQ* req);


} // namespace x509
} //namespace Cert

#endif
