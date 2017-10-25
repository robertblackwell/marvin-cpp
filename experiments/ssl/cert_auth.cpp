//
//  cert_auth.cpp
//  all
//
//  Created by ROBERT BLACKWELL on 10/22/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

/*ation. Demonstrates some certificate related
 * operations.
 */


#include <stdio.h>
#include <stdlib.h>

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif

#ifdef MKCERT
int mkcert(X509 **x509p, EVP_PKEY **pkeyp, int bits, int serial, int days);
int add_ext(X509 *cert, int nid, char *value);

int main(int argc, char **argv)
{
    BIO *bio_err;
    X509 *x509=NULL;
    EVP_PKEY *pkey=NULL;

    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

    bio_err=BIO_new_fp(stderr, BIO_NOCLOSE);

    mkcert(&x509,&pkey,512,0,365);

    RSA_print_fp(stdout,pkey->pkey.rsa,0);
    X509_print_fp(stdout,x509);

//    PEM_write_PrivateKey(stdout,pkey,NULL,NULL,0,NULL, NULL);
//    PEM_write_X509(stdout,x509);
//
    FILE* fout = fopen("/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/xxx_cert.pem", "w");
    PEM_write_PrivateKey(fout, pkey,NULL,NULL,0,NULL, NULL);
    PEM_write_X509(fout,x509);
    fclose(fout);


    X509_free(x509);
    EVP_PKEY_free(pkey);

#ifndef OPENSSL_NO_ENGINE
    ENGINE_cleanup();
#endif
    CRYPTO_cleanup_all_ex_data();

    CRYPTO_mem_leaks(bio_err);
    BIO_free(bio_err);
    return(0);
}

void callback(int p, int n, void *arg)
{
    char c='B';

    if (p == 0) c='.';
    if (p == 1) c='+';
    if (p == 2) c='*';
    if (p == 3) c='\n';
    fputc(c,stderr);
}

int mkcert(X509 **x509p, EVP_PKEY **pkeyp, int bits, int serial, int days)
{
    X509 *x;
    EVP_PKEY *pk;
    RSA *rsa;
    X509_NAME *name=NULL;
    
    if ((pkeyp == NULL) || (*pkeyp == NULL)) {
        if ((pk=EVP_PKEY_new()) == NULL) {
            abort();
            return(0);
        }
    } else {
        pk= *pkeyp;
    }
    if ((x509p == NULL) || (*x509p == NULL)) {
        if ((x=X509_new()) == NULL)
            throw "failed tp X509_new";
//            goto err;
    } else {
        x= *x509p;
    }
    rsa=RSA_generate_key(bits,RSA_F4,callback,NULL);
    if (!EVP_PKEY_assign_RSA(pk,rsa)){
        abort();
        throw "EVP_PKEY failed";
//        goto err;
    }
    rsa=NULL;

    X509_set_version(x,2);
    ASN1_INTEGER_set(X509_get_serialNumber(x),serial);
    X509_gmtime_adj(X509_get_notBefore(x),0);
    X509_gmtime_adj(X509_get_notAfter(x),(long)60*60*24*days);
    X509_set_pubkey(x,pk);

    name=X509_get_subject_name(x);

    /* This function creates and adds the entry, working out the
     * correct string type and performing checks on its length.
     * Normally we'd check the return value for errors...
     */
    const char* C = "C";
    const unsigned char* UK = (const unsigned char*)"UK";
    const char* CN = "CN";
    const unsigned char* OPENSSLGROUP = (const unsigned char*)"OpenSSL Group";
    
    X509_NAME_add_entry_by_txt(name,C,  MBSTRING_ASC, UK, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name,CN, MBSTRING_ASC, OPENSSLGROUP, -1, -1, 0);

    /* Its self signed so set the issuer name to be the same as the
      * subject.
     */
    X509_set_issuer_name(x,name);

    /* Add various extensions: standard extensions */
    add_ext(x, NID_basic_constraints, "critical,CA:TRUE");
    add_ext(x, NID_key_usage, "critical,keyCertSign,cRLSign");

    add_ext(x, NID_subject_key_identifier, "hash");

    /* Some Netscape specific extensions */
    add_ext(x, NID_netscape_cert_type, "sslCA");

    add_ext(x, NID_netscape_comment, "example comment extension");
    add_ext(x, NID_subject_alt_name, "asubjectaltname");
    add_ext(x, NID_subject_alt_name, "email:steve@openssl.org");

#define CUSTOM_EXT 1
#ifdef CUSTOM_EXT
    /* Maybe even add our own extension based on existing */
    {
        int nid;
        nid = OBJ_create("1.2.3.4", "MyAlias", "My Test Alias Extension");
        X509V3_EXT_add_alias(nid, NID_netscape_comment);
        add_ext(x, nid, "example comment alias");
    }
#endif
    
    if (!X509_sign(x,pk,EVP_md5()))
        goto err;

    *x509p=x;
    *pkeyp=pk;
    return(1);
err:
    return(0);
}

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */
int add_ext(X509 *cert, int nid, char *value)
{
    X509_EXTENSION *ex;
    X509V3_CTX ctx;
    /* This sets the 'context' of the extensions. */
    /* No configuration database */
    X509V3_set_ctx_nodb(&ctx);
    /* Issuer and subject certs: both the target since it is self signed,
     * no request and no CRL
     */
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
    if (!ex)
        return 0;

    X509_add_ext(cert,ex,-1);
    X509_EXTENSION_free(ex);
    return 1;
}
#endif

int mkreq(X509_REQ **x509p, EVP_PKEY **pkeyp, int bits, int serial, int days);
int add_ext(STACK_OF(X509_EXTENSION) *sk, int nid, char *value);
X509_REQ *req=NULL;

void doMakeRequest()
    {
    BIO *bio_err;
    EVP_PKEY *pkey=NULL;

    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

    bio_err=BIO_new_fp(stderr, BIO_NOCLOSE);

    mkreq(&req,&pkey,512,0,365);

    RSA_print_fp(stdout,pkey->pkey.rsa,0);
    X509_REQ_print_fp(stdout,req);

    PEM_write_X509_REQ(stdout,req);

//    X509_REQ_free(req);
    
    EVP_PKEY_free(pkey);

#ifndef OPENSSL_NO_ENGINE
    ENGINE_cleanup();
#endif
//    CRYPTO_cleanup_all_ex_data();
//
//    CRYPTO_mem_leaks(bio_err);
//    BIO_free(bio_err);
    }

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */

int add_ext(STACK_OF(X509_EXTENSION) *sk, int nid, char *value)
    {
    X509_EXTENSION *ex;
    ex = X509V3_EXT_conf_nid(NULL, NULL, nid, value);
    if (!ex)
        return 0;
    sk_X509_EXTENSION_push(sk, ex);

    return 1;
    }


static void callback(int p, int n, void *arg)
    {
    char c='B';

    if (p == 0) c='.';
    if (p == 1) c='+';
    if (p == 2) c='*';
    if (p == 3) c='\n';
    fputc(c,stderr);
    }

int mkreq(X509_REQ **req, EVP_PKEY **pkeyp, int bits, int serial, int days)
    {
    X509_REQ *x;
    EVP_PKEY *pk;
    RSA *rsa;
    X509_NAME *name=NULL;
    STACK_OF(X509_EXTENSION) *exts = NULL;
    
    if ((pk=EVP_PKEY_new()) == NULL)
        goto err;

    if ((x=X509_REQ_new()) == NULL)
        goto err;

    rsa=RSA_generate_key(bits,RSA_F4,callback,NULL);
    if (!EVP_PKEY_assign_RSA(pk,rsa))
        goto err;

    rsa=NULL;

    X509_REQ_set_pubkey(x,pk);

    name=X509_REQ_get_subject_name(x);

    /* This function creates and adds the entry, working out the
     * correct string type and performing checks on its length.
     * Normally we'd check the return value for errors...
     */
    X509_NAME_add_entry_by_txt(name,"C", MBSTRING_ASC, (const unsigned char*)"UK", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name,"CN", MBSTRING_ASC, (const unsigned char*)"OpenSSL Group", -1, -1, 0);

    /* Certificate requests can contain extensions, which can be used
     * to indicate the extensions the requestor would like added to
     * their certificate. CAs might ignore them however or even choke
     * if they are present.
     */

    /* For request extensions they are all packed in a single attribute.
     * We save them in a STACK and add them all at once later...
     */

    exts = sk_X509_EXTENSION_new_null();
    /* Standard extenions */

    add_ext(exts, NID_key_usage, "critical,digitalSignature,keyEncipherment");

    /* This is a typical use for request extensions: requesting a value for
     * subject alternative name.
     */

    add_ext(exts, NID_subject_alt_name, "email:steve@openssl.org");

    /* Some Netscape specific extensions */
    add_ext(exts, NID_netscape_cert_type, "client,email");
    add_ext(exts, NID_subject_alt_name, "email:steve@openssl.org");
    add_ext(exts, NID_subject_alt_name, "email:one.steve@openssl.org");
    add_ext(exts, NID_subject_alt_name, "email:two.steve@openssl.org");

    /* Now we've created the extensions we add them to the request */

    X509_REQ_add_extensions(x, exts);

    sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);

    if (!X509_REQ_sign(x,pk,EVP_sha1()))
        goto err;

    *req=x;
    *pkeyp=pk;
    return(1);
err:
    return(0);
    }

/* ------------------------------------------------------------ *
 * file:        certcreate.c                                    *
 * purpose:     Example code for creating OpenSSL certificates  *
 * author:      10/06/2012 Frank4DD                             *
 *                                                              *
 * gcc -o certcreate certcreate.c -lssl -lcrypto                *
 * ------------------------------------------------------------ */
#include <cstdlib>
#include <iostream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

/*********** where is the ca certificate .pem file ****************************/
#define CACERT          "/Users/rob/CA/private/cacert.pem"
/*********** where is the ca's private key file *******************************/
#define CAKEY           "/Users/rob/CA/private/cakey.pem"
/*********** The password for the ca's private key ****************************/
const std::string password("blackwellapps");

BIO               *reqbio = NULL;
BIO               *outbio = NULL;
X509                *cert = NULL;
X509_REQ         *certreq = NULL;

char request_str2[] =
"-----BEGIN CERTIFICATE REQUEST-----\n\
MIIBBDCBrwIBADBKMQswCQYDVQQGEwJKUDEOMAwGA1UECAwFVG9reW8xETAPBgNV\n\
BAoMCEZyYW5rNEREMRgwFgYDVQQDDA93d3cuZXhhbXBsZS5jb20wXDANBgkqhkiG\n\
9w0BAQEFAANLADBIAkEAm/xmkHmEQrurE/0re/jeFRLl8ZPjBop7uLHhnia7lQG/\n\
5zDtZIUC3RVpqDSwBuw/NTweGyuP+o8AG98HxqxTBwIDAQABoAAwDQYJKoZIhvcN\n\
AQEFBQADQQByOV52Y17y8xw1V/xvru3rLPrVxYAXS5SgvNpfBsj38lNVtTvuH/Mg\n\
roBgmjSpnqKqBiBDkoY2YUET2qmGjAu9\n\
-----END CERTIFICATE REQUEST-----";
char request_str[] =
"-----BEGIN CERTIFICATE REQUEST-----\n\
MIIDMDCCAhgCAQAwgZMxEDAOBgNVBAMMB3NzbHRlc3QxCzAJBgNVBAgMAldBMQsw\n\
CQYDVQQGEwJVUzEkMCIGCSqGSIb3DQEJARYVcm9iQGJsYWNrd2VsbGFwcHMuY29t\n\
MRYwFAYDVQQKDA1ibGFja3dlbGxhcHBzMScwJQYDVQQLDB5CbGFja3dlbGxBcHBz\n\
IFJvb3QgQ2VydGlmaWNhdGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\n\
AQCymr/H7vOg7E6lqH8X+yPmN09DnvlOnm3thM1LWghLplAdK3HFHHkEGOmYuey9\n\
4LLNtZHwYYedjuLTuaVMO6QQcgXzPCRizg9IATP2hs4XQVTC4ZSSVWl1RDERLu4W\n\
/Z2XTvBtuFOM92Luzt6D8OlLJ6LR9/pf3pkQ6048fzJvixY93BoEnsaUjpq0ZxGm\n\
HU5jgEh/ACb78AJVcWEDUOIrG44vSxqztyzDojSYE5S1zGGMiWy4QETwSoGKJUZN\n\
YVGjetJCRo1Zq0WUhL0A3GxSynT8g7UgEMTZfVCsOEnVIyMzNQvaapjqNBK0QBXV\n\
VE75eMtZtGKwJ9njcYgKlnEjAgMBAAGgVzBVBgkqhkiG9w0BCQ4xSDBGMAkGA1Ud\n\
EwQCMAAwCwYDVR0PBAQDAgWgMCwGA1UdEQQlMCOCB3NzbHRlc3SCC3d3dy5zc2x0\n\
ZXN0ggthcGkuc3NsdGVzdDANBgkqhkiG9w0BAQsFAAOCAQEAiBRpCO47T9QArufX\n\
B/Eqh1gSH4+WaernW5wZFhAdrMkarBq1FQGo6raO7miHzRAInb09MVA5GlRCDp0E\n\
kSe48Q0HbiUXZ7zh71ws4qh2W5u7JQkpQ5od/oxWLQsZ/7GwX3sctLfTgP4gNt8c\n\
NLPC0XDF6xEqm1wglFWCHKq2PG0af8C4Es55YRmp8QbMex4/fFysosm9Jd9bTKmt\n\
YpSvvXbJ/IVExAWDqyfPoFiUiYgUmDdb/uOkDBtgXNHfCBpS8wm5SaCG4GKAsJj5\n\
ePuU2hdM4j10m9HL4YTyfQuFvFbK2Za5AtAU2WgZTsxm8qmDYhynp1Y6CkucqJ7K\n\
/8S98Q==\n\
-----END CERTIFICATE REQUEST-----";


int password_cb(char *buf, int size, int rwflag, void *u)
{
    int len = (int)password.length();
    const char* passw = password.c_str();
    memcpy(buf, passw, len);
    return len;
}

int main() {

    ASN1_INTEGER                 *aserial = NULL;
    EVP_PKEY                     *ca_privkey, *req_pubkey;
    EVP_MD                       const *digest = NULL;
    X509                         *newcert, *cacert;
    X509_NAME                    *name;
    X509V3_CTX                   ctx;
    FILE                         *fp;
    long                         valid_secs = 31536000;
    
    doMakeRequest();
    
    /* ---------------------------------------------------------- *
    * These function calls initialize openssl for correct work.  *
    * ---------------------------------------------------------- */
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    /* ---------------------------------------------------------- *
    * Create the Input/Output BIO's.                             *
    * ---------------------------------------------------------- */
    outbio  = BIO_new(BIO_s_file());
    outbio = BIO_new_fp(stdout, BIO_NOCLOSE);

    /* ---------------------------------------------------------- *
    * Load the request data in a BIO, then in a x509_REQ struct. *
    * ---------------------------------------------------------- */
    reqbio = BIO_new_mem_buf(request_str, -1);
    
    if (! (certreq = PEM_read_bio_X509_REQ(reqbio, NULL, NULL, NULL))) {
        BIO_printf(outbio, "Error can't read X509 request data into memory\n");
        throw "failed to read req";
    }
    certreq = req;
    
    auto exts = X509_REQ_get_extensions(certreq);
    auto nn = sk_X509_EXTENSION_num(exts);
    
    auto nnn = X509at_get_attr_count(certreq->req_info->attributes);

// see  https://zakird.com/2013/10/13/certificate-parsing-with-openssl
// for X509* or certificates do the following
//STACK_OF(X509_EXTENSION) *exts = cert->cert_info->extensions;

    for(int i = 0 ; i < nn; i++) {
        X509_EXTENSION *ex = sk_X509_EXTENSION_value(exts, i);
        ASN1_OBJECT *obj = X509_EXTENSION_get_object(ex);
        BIO *ext_bio = BIO_new(BIO_s_mem());
        if (!X509V3_EXT_print(ext_bio, ex, 0, 0)) {
            M_ASN1_OCTET_STRING_print(ext_bio, ex->value);
        }
        BUF_MEM *bptr;

        BIO_get_mem_ptr(ext_bio, &bptr);
        BIO_set_close(ext_bio, BIO_NOCLOSE);
        // remove newlines
        int lastchar = bptr->length;
        if (lastchar > 1 && (bptr->data[lastchar-1] == '\n' || bptr->data[lastchar-1] == '\r')) {
            bptr->data[lastchar-1] = (char) 0;
        }
        if (lastchar > 0 && (bptr->data[lastchar] == '\n' || bptr->data[lastchar] == '\r')) {
            bptr->data[lastchar] = (char) 0;
        }

        BIO_free(ext_bio);

        unsigned nid = OBJ_obj2nid(obj);
        if (nid == NID_undef) {
            // no lookup found for the provided OID so nid came back as undefined.
            char extname[4096];
            OBJ_obj2txt(extname, 4096, (const ASN1_OBJECT *) obj, 1);
            printf("extension name is %s\n", extname);
        } else {
            // the OID translated to a NID which implies that the OID has a known sn/ln
            const char *c_ext_name = OBJ_nid2ln(nid);
            printf("extension name is %s\n", c_ext_name);
        }
        
        printf("extension length is %ul\n", bptr->length);
        printf("extension value is %s\n", bptr->data);
        
    }
    
    /* -------------------------------------------------------- *
    * Load ithe signing CA Certificate file                    *
    * ---------------------------------------------------------*/
    if (! (fp=fopen(CACERT, "r"))) {
        BIO_printf(outbio, "Error reading CA cert file\n");
        throw "failed to read CACERT";
    }

    if(! (cacert = PEM_read_X509(fp,NULL,NULL,NULL))) {
        BIO_printf(outbio, "Error loading CA cert into memory\n");
        exit(-1);
    }

    fclose(fp);

    /* -------------------------------------------------------- *
    * Import CA private key file for signing                   *
    * ---------------------------------------------------------*/
    ca_privkey = EVP_PKEY_new();

    if (! (fp = fopen (CAKEY, "r"))) {
        BIO_printf(outbio, "Error reading CA private key file\n");
        exit(-1);
    }

    if (! (ca_privkey = PEM_read_PrivateKey( fp, NULL, password_cb, NULL))) {
        BIO_printf(outbio, "Error importing key content from file\n");
        exit(-1);
    }

    fclose(fp);

    /* --------------------------------------------------------- *
    * Build Certificate with data from request                  *
    * ----------------------------------------------------------*/
    if (! (newcert=X509_new())) {
        BIO_printf(outbio, "Error creating new X509 object\n");
        exit(-1);
    }

    if (X509_set_version(newcert, 2) != 1) {
        BIO_printf(outbio, "Error setting certificate version\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * set the certificate serial number here                    *
    * If there is a problem, the value defaults to '0'          *
    * ----------------------------------------------------------*/
    aserial=M_ASN1_INTEGER_new();
    ASN1_INTEGER_set(aserial, 0);
    if (! X509_set_serialNumber(newcert, aserial)) {
        BIO_printf(outbio, "Error setting serial number of the certificate\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Extract the subject name from the request                 *
    * ----------------------------------------------------------*/
    if (! (name = X509_REQ_get_subject_name(certreq)))
        BIO_printf(outbio, "Error getting subject from cert request\n");

    /* --------------------------------------------------------- *
    * Set the new certificate subject name                      *
    * ----------------------------------------------------------*/
    if (X509_set_subject_name(newcert, name) != 1) {
        BIO_printf(outbio, "Error setting subject name of certificate\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Extract the subject name from the signing CA cert         *
    * ----------------------------------------------------------*/
    if (! (name = X509_get_subject_name(cacert))) {
        BIO_printf(outbio, "Error getting subject from CA certificate\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Set the new certificate issuer name                       *
    * ----------------------------------------------------------*/
    if (X509_set_issuer_name(newcert, name) != 1) {
        BIO_printf(outbio, "Error setting issuer name of certificate\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Extract the public key data from the request              *
    * ----------------------------------------------------------*/
    if (! (req_pubkey=X509_REQ_get_pubkey(certreq))) {
        BIO_printf(outbio, "Error unpacking public key from request\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Optionally: Use the public key to verify the signature    *
    * ----------------------------------------------------------*/
    if (X509_REQ_verify(certreq, req_pubkey) != 1) {
        BIO_printf(outbio, "Error verifying signature on request\n");
        exit(-1);
    }

    /* --------------------------------------------------------- *
    * Set the new certificate public key                        *
    * ----------------------------------------------------------*/
    if (X509_set_pubkey(newcert, req_pubkey) != 1) {
        BIO_printf(outbio, "Error setting public key of certificate\n");
        exit(-1);
    }

    /* ---------------------------------------------------------- *
    * Set X509V3 start date (now) and expiration date (+365 days)*
    * -----------------------------------------------------------*/
    if (! (X509_gmtime_adj(X509_get_notBefore(newcert),0))) {
        BIO_printf(outbio, "Error setting start time\n");
        exit(-1);
    }

    if(! (X509_gmtime_adj(X509_get_notAfter(newcert), valid_secs))) {
        BIO_printf(outbio, "Error setting expiration time\n");
        exit(-1);
    }

    /* ----------------------------------------------------------- *
    * Add X509V3 extensions                                       *
    * ------------------------------------------------------------*/
    X509V3_set_ctx(&ctx, cacert, newcert, NULL, NULL, 0);
    X509_EXTENSION *ext;
    auto req_sk = certreq->req_info->attributes;
    auto req_cert_altname = newcert->altname;
    auto cert_sk = newcert->cert_info->extensions;

    /* ----------------------------------------------------------- *
    * Set digest type, sign new certificate with CA's private key *
    * ------------------------------------------------------------*/
    digest = EVP_sha256();

    if (! X509_sign(newcert, ca_privkey, digest)) {
        BIO_printf(outbio, "Error signing the new certificate\n");
        exit(-1);
    }

    /* ------------------------------------------------------------ *
    *  print the certificate                                       *
    * -------------------------------------------------------------*/
    if (! PEM_write_bio_X509(outbio, newcert)) {
        BIO_printf(outbio, "Error printing the signed certificate\n");
        exit(-1);
    }
    X509_print_fp(stdout, newcert);

    /* ---------------------------------------------------------- *
    * Free up all structures                                     *
    * ---------------------------------------------------------- */

    EVP_PKEY_free(req_pubkey);
    EVP_PKEY_free(ca_privkey);
    X509_REQ_free(certreq);
    X509_free(newcert);
    BIO_free_all(reqbio);
    BIO_free_all(outbio);
    exit(0);
}

