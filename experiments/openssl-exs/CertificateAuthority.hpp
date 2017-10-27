//
//  CertificateAuthority.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 10/25/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef CertificateAuthority_hpp
#define CertificateAuthority_hpp

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "x509.hpp"

class CertificateAuthority
{
    public:
        CertificateAuthority(std::string caDirectory);
        ~CertificateAuthority();
        X509*       getCACert();
        EVP_PKEY*   getCAPKey();
        int         getNextSerialNumber();
    private:
        std::string _caDirectory;
        std::string _caCertFileName;
        std::string _caPKeyFileName;
        std::string _serialNumberFileName;
    
        X509*       _caCert;
        EVP_PKEY*   _caPKey;
        int         nextSerialNumber;
};
#endif /* CertificateAuthority_hpp */
