//
//  CertificateBuilder.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/25/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include "x509_error.hpp"
#include "CertificateBuilder.hpp"

X509*
buildCertificate(X509* CAcert, EVP_PKEY* CApkey, int serial, X509_REQ* req, X509* original_cert, AdditionalExtensions extensions)
{
    EVP_PKEY *pubkey;
    const EVP_MD *digest;
    X509 *cert;
    X509_NAME *name;
    X509_EXTENSION *subjAltName;
    BIO *out;

    if ( (original_cert == NULL) && (req == NULL))
        throw x509Exception("cannot have both req and original_cert as NULL");
    
    if ( (original_cert != NULL) && (req != NULL))
        throw x509Exception("cannot have both req and original_cert as NOT NULL");

    bool additionalExtensions = (extensions.size() > 0);
    
    /* open stdout */
    if (!(out = BIO_new_fp (stdout, BIO_NOCLOSE)))
        X509_TRIGGER_ERROR ("Error creating stdout BIO");

//    req = x509Req_ReadFromFile(REQ_FILE);
//    x509Req_VerifySignature(req);
//    CAcert = x509Cert_ReadFromFile(CA_FILE);
//    CApkey = x509PKey_ReadPrivateKeyFrom(CA_KEY, const_password);
//    original_cert = x509Cert_ReadFromFile(ORIGINAL_CERT_FILE);
    
    x509Req_PrintNames(out, req);
    
    name = x509Req_GetSubjectName(req);
    subjAltName = x509Req_GetSubjectAltName(req);
    
    X509_NAME_print (out, name, 0);
    fputc ('\n', stdout);
    X509V3_EXT_print (out, subjAltName, 0, 0);
    fputc ('\n', stdout);

    pubkey = X509_REQ_get_pubkey (req);
    if (!(cert = X509_new ()))
        X509_TRIGGER_ERROR ("Error creating X509 object");

    x509Cert_SetVersion(cert, 2L);
    x509Cert_SetSerialNumber(cert, serial);
    x509Cert_SetPubKey(cert, pubkey);

    x509Cert_SetIssuerName(cert, x509Cert_GetSubjectName(CAcert));
    //
    // THese next two calls a kluges as we shouodl pass this stuff in a arguments
    //
    x509Cert_SetNotBefore(cert, -(60*60*24*365));
    x509Cert_SetNotAfter(cert, (60*60*24*365*5));
    /*
    ** Now add extension including subjectAltNames
    */
    if( req != NULL ) {
        X509_EXTENSION *subjAltName;
        
        x509Cert_SetSubjectName(cert, x509Req_GetSubjectName(req));
        subjAltName = x509Req_GetSubjectAltName(req);
        x509Cert_AddExtension(cert, subjAltName);
        if ( additionalExtensions ) {
            x509Cert_Add_ExtensionsFromTable(CAcert, cert, extensions);
        } else /*take extensions from req*/{
            STACK_OF(X509_EXTENSION)* stack = X509_REQ_get_extensions(req);
            x509Cert_Add_ExtensionsFromStack(CAcert, cert, stack);
        }
    } else { // get extensions from original cert
        X509_EXTENSION *subjAltName;
        x509Cert_SetSubjectName(cert, x509Cert_GetSubjectName(original_cert));
        subjAltName = x509Cert_GetSubjectAltName(original_cert);

        x509Cert_AddExtension(cert, subjAltName);
        
        STACK_OF(X509_EXTENSION*) stack = x509Cert_GetExtensions(original_cert);
        x509Cert_Add_ExtensionsFromStack(CAcert, cert, stack);
    }
    
    /*
    ** select the digest to use for signing. Sicne we only use RSA keys
    ** and we want to ALWAYS use sha256 - ther is no decision
    */
    if (EVP_PKEY_type (CApkey->type) == EVP_PKEY_RSA)
        digest = EVP_sha1();
    else
        X509_TRIGGER_ERROR ("Error  CA private key is NOT RSA");
    /*
    ** The big moment sign the cert
    ** sign the certificate with the CA private key
    */
    if (!(X509_sign (cert, CApkey, digest)))
        X509_TRIGGER_ERROR ("Error signing certificate");

    return cert;
}

X509* buildCertificate(X509* CAcert, EVP_PKEY* CApkey, int serial, X509_REQ* req)
{
    AdditionalExtensions emptyExts;
    return buildCertificate(CAcert, CApkey, serial, req, nullptr, emptyExts);
}

X509* buildCertificate(X509* CAcert, EVP_PKEY* CApkey, int serial, X509_REQ* req, AdditionalExtensions  extensions)
{
    return buildCertificate(CAcert, CApkey, serial, req, nullptr, extensions);
}

X509* buildCertificate(X509* CAcert, EVP_PKEY* CApkey, int serial, X509* original_cert)
{
    AdditionalExtensions emptyExts;
    return buildCertificate(CAcert, CApkey, serial, nullptr, original_cert, emptyExts);
}


X509* CertificateBuilder::buildFromReq(X509_REQ* req)
{
    return buildCertificate(_certAuth.getCACert(), _certAuth.getCAPKey(), _certAuth.getNextSerialNumber(), req);
}

X509* CertificateBuilder::buildFromReq(X509_REQ* req, AdditionalExtensions extensions)
{
    return buildCertificate(_certAuth.getCACert(), _certAuth.getCAPKey(), _certAuth.getNextSerialNumber(), req, extensions);
}

X509* CertificateBuilder::buildFromCert(X509* original_cert)
{
    return buildCertificate(_certAuth.getCACert(), _certAuth.getCAPKey(), _certAuth.getNextSerialNumber(), original_cert);
}
