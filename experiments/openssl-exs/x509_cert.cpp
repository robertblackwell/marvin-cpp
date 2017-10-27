//
//  x509_cert.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_cert.hpp"
#include "x509_error.hpp"
//
//  x509_ext.cpp
//  all
//
//  Created by ROBERT BLACKWELL on 10/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//


#pragma mark - x509Cert funuctions
X509*
x509Cert_ReadFromFile(std::string fileName)
{
    X509* cert;
    char* fn = (char*)fileName.c_str();
    FILE* fp;
    
    if (!(fp = fopen (fn, "r")))
        X509_TRIGGER_ERROR ("Error reading CA certificate file");
    if (!(cert = PEM_read_X509 (fp, NULL, NULL, NULL)))
        X509_TRIGGER_ERROR ("Error reading CA certificate in file");
    fclose (fp);
    return cert;
}
void
x509Cert_WriteToFile(X509* cert, std::string fileName)
{
    FILE* fp;
    char* fn = (char*) fileName.c_str();
    if (!(fp = fopen (fn, "w")))
        X509_TRIGGER_ERROR ("Error writing to certificate file");
    if (PEM_write_X509 (fp, cert) != 1)
        X509_TRIGGER_ERROR ("Error while writing certificate");
    fclose (fp);
}
void
x509Cert_AddExtension(X509* cert, X509_EXTENSION* ext)
{
    /* add the subjectAltName in the request to the cert */
    if (!X509_add_ext (cert, ext, -1))
        X509_TRIGGER_ERROR ("Error adding subjectAltName to certificate");
}

X509_NAME*
x509Cert_GetSubjectName(X509* cert)
{
    X509_NAME *name;
    if (!(name = X509_get_subject_name (cert)))
        X509_TRIGGER_ERROR ("Error getting subject name from request");
    return name;
}
void
x509Cert_SetSubjectName(X509* cert, X509_NAME* name)
{
    if (X509_set_subject_name (cert, name) != 1)
        X509_TRIGGER_ERROR ("Error setting subject name of certificate");
}
void
x509Cert_SetIssuerName(X509* cert, X509_NAME* name)
{
    if (X509_set_issuer_name (cert, name) != 1)
        X509_TRIGGER_ERROR ("Error setting issuer name of certificate");
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
//        X509_TRIGGER_ERROR ("Error getting the request's extensions");
//    subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
//                       OBJ_sn2nid ("subjectAltName"), -1);
//    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
//    return subjAltName;

}

void
x509Cert_SetVersion(X509* cert, long version)
{
    if (X509_set_version (cert, 2L) != 1)
        X509_TRIGGER_ERROR ("Error settin certificate version");
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
        X509_TRIGGER_ERROR ("Error setting public key of the certificate");
}
void
x509Cert_SetNotBefore(X509* cert, int secs)
{
    /* set duration for the certificate */
    if (!(X509_gmtime_adj (X509_get_notBefore (cert), secs)))
        X509_TRIGGER_ERROR ("Error setting beginning time of the certificate");
}
void
x509Cert_SetNotAfter(X509* cert, int secs)
{
    if (!(X509_gmtime_adj (X509_get_notAfter (cert), secs)))
        X509_TRIGGER_ERROR ("Error setting ending time of the certificate");
}
void
x509Cert_SetPublicKey(X509* cert, EVP_PKEY* pubkey)
{
    /* set public key in the certificate */
    if (X509_set_pubkey (cert, pubkey) != 1)
        X509_TRIGGER_ERROR ("Error setting public key of the certificate");
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
            X509_TRIGGER_ERROR ("Error creating X509 extension object");
        }
        if (!X509_add_ext (cert, ext, -1))
        {
            fprintf (stderr, "Error on \"%s = %s\"\n",  k, v);
            X509_TRIGGER_ERROR ("Error adding X509 extension to certificate");
        }
        X509_EXTENSION_free (ext);
    }
}
void
x509Cert_Add_ExtensionsFromStack(X509* CAcert, X509* cert, STACK_OF(X509_EXTENSION)* stack)
{
    int nbr = sk_X509_EXTENSION_num(stack);
    for(int i = 0; i < nbr; i++) {
        auto ext = sk_X509_EXTENSION_value(stack, i);
        auto ext_dup = X509_EXTENSION_dup(ext);
        if (!X509_add_ext (cert, ext_dup, -1))
        {
            X509_TRIGGER_ERROR ("Error adding X509 extension to certificate");
        }
        X509_EXTENSION_free (ext_dup);
    }
}


