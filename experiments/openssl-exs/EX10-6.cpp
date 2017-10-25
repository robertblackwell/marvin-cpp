#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "x509_ext.hpp"

//
// all of these globals need to be parameters or properties
//
const std::string CA_FILE("/Users/rob/CA/private/cacert.pem");
const std::string CA_KEY("/Users/rob/CA/private/cakey.pem");
const std::string REQ_FILE("/Users/rob/CA/sites/ssltest/request.pem");
const std::string CERT_FILE("/Users/rob/CA/sites/ssltest/newcert.pem");
const std::string ORIGINAL_CERT_FILE("/Users/rob/CA/certs/www.google.com/real_certificate.pem");

const int days_till_expiry = 365;
const int expiry_secs = (60*60*24*days_till_expiry);

std::map<std::string, std::string> extensions = {
  {"basicConstraints", "CA:FALSE"},
  {"nsComment", "\"XXOpenSSL Generated Certificate\""},
  {"subjectKeyIdentifier", "hash"},
  {"authorityKeyIdentifier", "keyid,issuer:always"},
  {"keyUsage", "nonRepudiation,digitalSignature,keyEncipherment"}
};

static std::string const_password("blackwellapps");
const bool usingReqForNames = true; // subject and alt names from request or original cert
const bool takeExtensionsFromTable = false;// extensions from table , request or original cert
long serial = 1; // next serial number

//
// @TODO - test getting extensions from orginal cert
// @TODO - check we are free'ing everything required
//

int
main (int argc, char *argv[])
{
    EVP_PKEY *pubkey, *CApkey;
    const EVP_MD *digest;
    X509 *cert, *CAcert;
    X509* original_cert;
    X509_REQ *req;
    X509_NAME *name;
    X509_EXTENSION *subjAltName;
    BIO *out;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    /* open stdout */
    if (!(out = BIO_new_fp (stdout, BIO_NOCLOSE)))
        int_error ("Error creating stdout BIO");

    req = x509Req_ReadFromFile(REQ_FILE);
    x509Req_VerifySignature(req);
    CAcert = x509Cert_ReadFromFile(CA_FILE);
    CApkey = x509PKey_ReadPrivateKeyFrom(CA_KEY, const_password);
    original_cert = x509Cert_ReadFromFile(ORIGINAL_CERT_FILE);
    
    x509Req_PrintNames(out, req);
    name = x509Req_GetSubjectName(req);
    subjAltName = x509Req_GetSubjectAltName(req);
    
    X509_NAME_print (out, name, 0);
    fputc ('\n', stdout);
    X509V3_EXT_print (out, subjAltName, 0, 0);
    fputc ('\n', stdout);

    pubkey = X509_REQ_get_pubkey (req);
    if (!(cert = X509_new ()))
        int_error ("Error creating X509 object");

    x509Cert_SetVersion(cert, 2L);
    x509Cert_SetSerialNumber(cert, serial++);
    x509Cert_SetPubKey(cert, pubkey);

    x509Cert_SetIssuerName(cert, x509Cert_GetSubjectName(CAcert));
    x509Cert_SetNotBefore(cert, 0);
    x509Cert_SetNotAfter(cert, expiry_secs);
    /*
    ** Now add extension including subjectAltNames
    */
    if( usingReqForNames ) {
        X509_EXTENSION *subjAltName;
        
        x509Cert_SetSubjectName(cert, x509Req_GetSubjectName(req));
        subjAltName = x509Req_GetSubjectAltName(req);
        x509Cert_AddExtension(cert, subjAltName);
        if (takeExtensionsFromTable ) {
            x509Cert_Add_ExtensionsFromTable(CAcert, cert, extensions);
        } else /*take extensions from req*/{
            STACK_OF(X509_EXTENSION*) stack = X509_REQ_get_extensions(req);
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
        int_error ("Error  CA private key is NOT RSA");
    /*
    ** The big moment sign the cert
    ** sign the certificate with the CA private key
    */
    if (!(X509_sign (cert, CApkey, digest)))
        int_error ("Error signing certificate");


    x509Cert_WriteToFile(cert, CERT_FILE);
    return 0;
}
