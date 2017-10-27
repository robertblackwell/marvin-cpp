//
//  CertificateBuilder.hpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/25/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef CertificateBuilder_hpp
#define CertificateBuilder_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "CertificateAuthority.hpp"

typedef std::map<std::string, std::string> AdditionalExtensions;

class CertificateBuilder
{
    public:
    CertificateBuilder(CertificateAuthority& certAuthority) : _certAuth(certAuthority)
    {
    }
    
    X509* buildFromReq(X509_REQ* req);
    X509* buildFromReq(X509_REQ* req, AdditionalExtensions extensions);
    X509* buildFromCert(X509* original_cert);
    private:
        CertificateAuthority& _certAuth;
//        std::string _caKeyPassword;
//        std::string _caCertFn;
//        std::string _caPKeyFn;
//        int         _serialNumber;
//        X509*       _CAcert;
//        EVP_PKEY*   _CApkey;
//        X509*       _cert;
//        X509*       _original_cert;
};

#endif /* CertificateBuilder_hpp */
