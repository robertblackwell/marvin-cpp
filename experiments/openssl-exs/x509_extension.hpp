//
//  x509_extension.hpp
//  req_test
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef x509_extension_hpp
#define x509_extension_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>


typedef STACK_OF(X509_EXTENSION)* ExtensionStack;

ExtensionStack x509ExtensionStack_New();
void x509ExtensionStack_Free();

void x509ExtensionStack_AddByNID(STACK_OF(X509_EXTENSION) *stack, int nid, std::string value);
void x509ExtensionStack_AddBySN(STACK_OF(X509_EXTENSION) *stack, std::string sn_string, std::string value );


#endif /* x509_extension_hpp */
