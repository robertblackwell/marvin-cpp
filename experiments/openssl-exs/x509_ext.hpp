//
//  x509_ext.hpp
//  all
//
//  Created by ROBERT BLACKWELL on 10/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef x509_ext_hpp
#define x509_ext_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)
void
handle_error (const char *file, int lineno, const char *msg);


EVP_PKEY*
x509PKey_ReadPrivateKeyFrom(std::string fileName, std::string password);

EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName);

#pragma mark - x509Cert funuctions
X509*
x509Cert_ReadFromFile(std::string fileName);

void
x509Cert_WriteToFile(X509* cert, std::string fileName);

void
x509Cert_AddExtension(X509* cert, X509_EXTENSION* ext);

X509_NAME*
x509Cert_GetSubjectName(X509* cert);

void
x509Cert_SetSubjectName(X509* cert, X509_NAME* name);

void
x509Cert_SetIssuerName(X509* cert, X509_NAME* name);

STACK_OF(X509_EXTENSION)*
x509Cert_GetExtensions(X509* cert);

X509_EXTENSION*
x509Cert_GetSubjectAltName(X509* cert);

void
x509Cert_SetVersion(X509* cert, long version);

void
x509Cert_SetSerialNumber(X509* cert, long serial);

void
x509Cert_SetPubKey(X509* cert, EVP_PKEY* pubkey);

void
x509Cert_SetNotBefore(X509* cert, int secs);

void
x509Cert_SetNotAfter(X509* cert, int secs);

void
x509Cert_SetPublicKey(X509* cert, EVP_PKEY* pubkey);

void
x509Cert_Add_ExtensionsFromTable(X509* CAcert, X509* cert, std::map<std::string, std::string> table);

void
x509Cert_Add_ExtensionsFromStack(X509* CAcert, X509* cert, STACK_OF(X509_EXTENSION*) stack);

#pragma mark x509Req functions

void
x509Req_VerifySignature(X509_REQ* req);

X509_REQ*
x509Req_ReadFromFile(std::string fileName);

X509_NAME*
x509Req_GetSubjectName(X509_REQ* req);

X509_EXTENSION*
x509Req_GetSubjectAltName(X509_REQ* req);

void x509Req_PrintNames(BIO* out, X509_REQ* req);



#endif /* x509_ext_hpp */
