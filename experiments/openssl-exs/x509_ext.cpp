//
//  x509_ext.cpp
//  all
//
//  Created by ROBERT BLACKWELL on 10/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_ext.hpp"
//#include <cstdlib>
//#include <iostream>
//#include <map>
//#include <vector>
//#include <openssl/x509.h>
//#include <openssl/x509v3.h>
//#include <openssl/err.h>
//#include <openssl/pem.h>
//#include <openssl/rand.h>


void
handle_error (const char *file, int lineno, const char *msg)
{
  fprintf (stderr, "** %s:%i %s\n", file, lineno, msg);
  ERR_print_errors_fp (stderr);
  throw msg;
//  exit (-1);
}

#pragma mark - loadin private keys
std::string const_password("blackwellapps");
typedef int (*PemPasswordCb)(char *buf, int size, int rwflag, void *u);

int password_cb(char *buf, int size, int rwflag, void *u)
{
    std::string s(const_password);
    const char* c_s = s.c_str();
    std::strcpy(buf, c_s);
    return (int)s.length();
}

EVP_PKEY*
x509PKey_ReadPrivateKeyFrom(std::string fileName, std::string password)
{
    /*
    * Type conversion nonsense - because C code does not convert well to C++
    */
    EVP_PKEY** dummyIn = nullptr;
    EVP_PKEY *pkey;
    FILE* fp;
    char* fileNameCStr = (char*)fileName.c_str();
    
    if (!(fp = fopen (fileNameCStr, "r")))
        int_error( "ReadPrivateKeyFrom:: failed to open key file");

// have not implemented the callback version of loading a private key

#define PWCB_NO
#ifdef PWCB
    if (!(pkey = PEM_read_PrivateKey (fp, dummyIn, &password_cb, NULL)))
        int_error ("ReadPrivateKey::Error reading private key in file");
#else
    PemPasswordCb nullCb = nullptr;
    void* pw = (void*) password.c_str();
    if (!(pkey = PEM_read_PrivateKey (fp, dummyIn, nullCb, pw)))
        int_error ("ReadPrivateKey::Error reading private key in file");
#endif
    fclose(fp);
    
    return pkey;
}
EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName)
{
    throw "not implemented";
    return x509PKey_ReadPrivateKeyFrom(fileName, const_password);
}
#pragma mark - x509Cert funuctions
X509*
x509Cert_ReadFromFile(std::string fileName)
{
    X509* cert;
    char* fn = (char*)fileName.c_str();
    FILE* fp;
    
    if (!(fp = fopen (fn, "r")))
        int_error ("Error reading CA certificate file");
    if (!(cert = PEM_read_X509 (fp, NULL, NULL, NULL)))
        int_error ("Error reading CA certificate in file");
    fclose (fp);
    return cert;
}
void
x509Cert_WriteToFile(X509* cert, std::string fileName)
{
    FILE* fp;
    char* fn = (char*) fileName.c_str();
    if (!(fp = fopen (fn, "w")))
        int_error ("Error writing to certificate file");
    if (PEM_write_X509 (fp, cert) != 1)
        int_error ("Error while writing certificate");
    fclose (fp);
}
void
x509Cert_AddExtension(X509* cert, X509_EXTENSION* ext)
{
    /* add the subjectAltName in the request to the cert */
    if (!X509_add_ext (cert, ext, -1))
        int_error ("Error adding subjectAltName to certificate");
}

X509_NAME*
x509Cert_GetSubjectName(X509* cert)
{
    X509_NAME *name;
    if (!(name = X509_get_subject_name (cert)))
        int_error ("Error getting subject name from request");
    return name;
}
void
x509Cert_SetSubjectName(X509* cert, X509_NAME* name)
{
    if (X509_set_subject_name (cert, name) != 1)
        int_error ("Error setting subject name of certificate");
}
void
x509Cert_SetIssuerName(X509* cert, X509_NAME* name)
{
    if (X509_set_issuer_name (cert, name) != 1)
        int_error ("Error setting issuer name of certificate");
}

STACK_OF(X509_EXTENSION)*
x509Cert_GetExtensions(X509* cert)
{
    auto x = cert->cert_info->extensions;
    return x;
}
X509_EXTENSION*
x509Cert_GetSubjectAltName(X509* cert)
{
return NULL;
//    X509_EXTENSION *subjAltName;
//    STACK_OF (X509_EXTENSION) * req_exts;
//    int subjAltName_pos;
//    if (!(req_exts = X509_get_extensions (cert)))
//        int_error ("Error getting the request's extensions");
//    subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
//                       OBJ_sn2nid ("subjectAltName"), -1);
//    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
//    return subjAltName;

}

void
x509Cert_SetVersion(X509* cert, long version)
{
    if (X509_set_version (cert, 2L) != 1)
        int_error ("Error settin certificate version");
}
void
x509Cert_SetSerialNumber(X509* cert, long serial)
{
    ASN1_INTEGER_set (X509_get_serialNumber (cert), serial);
}
void
x509Cert_SetPubKey(X509* cert, EVP_PKEY* pubkey)
{
    /* set public key in the certificate */
    if (X509_set_pubkey (cert, pubkey) != 1)
        int_error ("Error setting public key of the certificate");
}
void
x509Cert_SetNotBefore(X509* cert, int secs)
{
    /* set duration for the certificate */
    if (!(X509_gmtime_adj (X509_get_notBefore (cert), secs)))
        int_error ("Error setting beginning time of the certificate");
}
void
x509Cert_SetNotAfter(X509* cert, int secs)
{
    if (!(X509_gmtime_adj (X509_get_notAfter (cert), secs)))
        int_error ("Error setting ending time of the certificate");
}
void
x509Cert_SetPublicKey(X509* cert, EVP_PKEY* pubkey)
{
    /* set public key in the certificate */
    if (X509_set_pubkey (cert, pubkey) != 1)
        int_error ("Error setting public key of the certificate");
}

void
x509Cert_Add_ExtensionsFromTable(X509* CAcert, X509* cert, std::map<std::string, std::string> table)
{
    X509V3_CTX ctx;
    X509V3_set_ctx (&ctx, CAcert, cert, NULL, NULL, 0);
    for( auto & x : table) {
        auto k = (char*)x.first.c_str();
        auto v = (char*)x.second.c_str();
        X509_EXTENSION *ext;
        ext = X509V3_EXT_conf (NULL, &ctx, k, v);
        if (!ext )
        {
            fprintf (stderr, "Error on \"%s = %s\"\n", k, v);
            int_error ("Error creating X509 extension object");
        }
        if (!X509_add_ext (cert, ext, -1))
        {
            fprintf (stderr, "Error on \"%s = %s\"\n",  k, v);
            int_error ("Error adding X509 extension to certificate");
        }
        X509_EXTENSION_free (ext);
    }
}
void
x509Cert_Add_ExtensionsFromStack(X509* CAcert, X509* cert, STACK_OF(X509_EXTENSION*) stack)
{
    int nbr = sk_X509_EXTENSION_num(stack);
    for(int i = 0; i < nbr; i++) {
        auto ext = sk_X509_EXTENSION_value(stack, i);
        auto ext_dup = X509_EXTENSION_dup(ext);
        if (!X509_add_ext (cert, ext_dup, -1))
        {
            int_error ("Error adding X509 extension to certificate");
        }
        X509_EXTENSION_free (ext_dup);
    }
}

#pragma mark x509Req functions

void
x509Req_VerifySignature(X509_REQ* req)
{
    EVP_PKEY* pkey;
    /* verify signature on the request */
    if (!(pkey = X509_REQ_get_pubkey (req)))
        int_error ("Error getting public key from request");
    if (X509_REQ_verify (req, pkey) != 1)
        int_error ("Error verifying signature on certificate");
}

X509_REQ*
x509Req_ReadFromFile(std::string fileName)
{
    X509_REQ* req;
    FILE* fp;
    char* fn = (char*)fileName.c_str();
    if (!(fp = fopen (fn, "r")))
        int_error ("Error reading request file");
    if (!(req = PEM_read_X509_REQ (fp, NULL, NULL, NULL)))
        int_error ("Error reading request in file");
    fclose (fp);
    x509Req_VerifySignature(req);
    return req;
}

X509_NAME*
x509Req_GetSubjectName(X509_REQ* req)
{
    X509_NAME *name;
    if (!(name = X509_REQ_get_subject_name (req)))
        int_error ("Error getting subject name from request");
    return name;
}
X509_EXTENSION*
x509Req_GetSubjectAltName(X509_REQ* req)
{
    X509_EXTENSION *subjAltName;
    STACK_OF (X509_EXTENSION) * req_exts;
    int subjAltName_pos;
    if (!(req_exts = X509_REQ_get_extensions (req)))
        int_error ("Error getting the request's extensions");
    subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
                       OBJ_sn2nid ("subjectAltName"), -1);
    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
    return subjAltName;
}

void x509Req_PrintNames(BIO* out, X509_REQ* req)
{
  X509_NAME *name;
  X509_EXTENSION *subjAltName;
  STACK_OF (X509_EXTENSION) * req_exts;
  int subjAltName_pos;

  if (!(name = X509_REQ_get_subject_name (req)))
    int_error ("Error getting subject name from request");
  X509_NAME_print (out, name, 0);
  fputc ('\n', stdout);
  if (!(req_exts = X509_REQ_get_extensions (req)))
    int_error ("Error getting the request's extensions");
  subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
                       OBJ_sn2nid ("subjectAltName"), -1);
  subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
  X509V3_EXT_print (out, subjAltName, 0, 0);
  fputc ('\n', stdout);
}


