//
//  x509_pkey.hpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef x509_pkey_hpp
#define x509_pkey_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

EVP_PKEY* x509Rsa_Generate();

EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName, std::string password);

EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName);

void x509PKey_WritePrivateKey(EVP_PKEY* pkey, std::string filename, std::string password);


#endif /* x509_pkey_hpp */
