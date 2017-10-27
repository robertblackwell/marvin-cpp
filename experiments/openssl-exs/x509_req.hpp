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
#include "x509_extension.hpp"


#pragma mark x509Req functions

X509_REQ* x509Req_New();

void X508Req_AddExtensions(X509_REQ* req, ExtensionStack);

void x509Req_SetPublicKey(X509_REQ* req, EVP_PKEY* pkey );
void x509Req_SetSubjectName(X509_REQ* req, std::map<std::string, std::string> subjectValues);


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
