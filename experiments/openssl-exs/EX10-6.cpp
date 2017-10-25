#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>


void
handle_error (const char *file, int lineno, const char *msg)
{
  fprintf (stderr, "** %s:%i %s\n", file, lineno, msg);
  ERR_print_errors_fp (stderr);
  throw msg;
//  exit (-1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)

/* these are defintions to make the example simpler */
//#define CA_FILE "/Users/rob/CA/private/cacert.pem"
const std::string CA_FILE("/Users/rob/CA/private/cacert.pem");
char* ca_file = (char*)CA_FILE.c_str();

const std::string CA_KEY("/Users/rob/CA/private/cakey.pem");
char* ca_key = (char*)CA_KEY.c_str();

const std::string  REQ_FILE("/Users/rob/CA/sites/ssltest/request.pem");
char* req_file = (char*)REQ_FILE.c_str();

const std::string  CERT_FILE("/Users/rob/CA/sites/ssltest/newcert.pem");
char* cert_file = (char*)CERT_FILE.c_str();

const std::string  ORIGINAL_CERT("/Users/rob/CA/certs/www.google.com/real_certificate.pem");
char* original_cert = (char*)ORIGINAL_CERT.c_str();

#define DAYS_TILL_EXPIRE 365
#define EXPIRE_SECS (60*60*24*DAYS_TILL_EXPIRE)

#define EXT_COUNT 5



struct entry
{
  char *key;
  char *value;
};

struct entry ext_ent[EXT_COUNT] = {
  {"basicConstraints", "CA:FALSE"},
  {"nsComment", "\"XXOpenSSL Generated Certificate\""},
  {"subjectKeyIdentifier", "hash"},
  {"authorityKeyIdentifier", "keyid,issuer:always"},
  {"keyUsage", "nonRepudiation,digitalSignature,keyEncipherment"}
};

std::map<std::string, std::string> extensions = {
  {"basicConstraints", "CA:FALSE"},
  {"nsComment", "\"XXOpenSSL Generated Certificate\""},
  {"subjectKeyIdentifier", "hash"},
  {"authorityKeyIdentifier", "keyid,issuer:always"},
  {"keyUsage", "nonRepudiation,digitalSignature,keyEncipherment"}
};

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
EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName, std::string password)
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
    
#define PWCB
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
    if (!(fp = fopen (cert_file, "w")))
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
    int i;
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

#pragma mark - main

int
main (int argc, char *argv[])
{
    /*
    ** set this false to take the subjectName and subkectAltnames from the original cert
    */
    const bool usingReqForNames = true;
    /*
    ** When the following is true and usingReqForNames is true we get the extension to use for the new cert from
    ** a table/data structure NOT from the req.
    */
    const bool takeExtensionsFromTable = true;
    
    int i, subjAltName_pos;
    long serial = 1;
    EVP_PKEY *pubkey, *CApkey;
    const EVP_MD *digest;
    X509 *cert, *CAcert;
    X509* original_cert;
    X509_REQ *req;
    X509V3_CTX ctx;
    X509_NAME *name;
    X509_EXTENSION *subjAltName;
    STACK_OF (X509_EXTENSION) * req_exts;
    FILE *fp;
    BIO *out;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    /* open stdout */
    if (!(out = BIO_new_fp (stdout, BIO_NOCLOSE)))
        int_error ("Error creating stdout BIO");
#define NEWSTUFF 1
#ifdef NEWSTUFF
    req = x509Req_ReadFromFile(REQ_FILE);
    x509Req_VerifySignature(req);
    CAcert = x509Cert_ReadFromFile(CA_FILE);
    CApkey = x509PKey_ReadPrivateKeyFrom(CA_KEY);
    original_cert = x509Cert_ReadFromFile(ORIGINAL_CERT);
    auto xx = x509Cert_GetExtensions(original_cert);
    
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
//    /* set public key in the certificate */
//    if (X509_set_pubkey (cert, pubkey) != 1)
//        int_error ("Error setting public key of the certificate");


    x509Cert_SetIssuerName(cert, x509Cert_GetSubjectName(CAcert));
    x509Cert_SetNotBefore(cert, 0);
    x509Cert_SetNotAfter(cert, EXPIRE_SECS);
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
            x509Cert_Add_ExtensionsFromStack(CAcert, cert,stack);
        }
    } else {
        X509_EXTENSION *subjAltName;
        subjAltName = x509Cert_GetSubjectAltName(original_cert);

        x509Cert_SetSubjectName(cert, x509Req_GetSubjectName(req));
        x509Cert_AddExtension(cert, subjAltName);
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
#else
    /* read in the request */
    if (!(fp = fopen (req_file, "r")))
        int_error ("Error reading request file");
    if (!(req = PEM_read_X509_REQ (fp, NULL, NULL, NULL)))
        int_error ("Error reading request in file");
    fclose (fp);

    /* verify signature on the request */
    if (!(pubkey = X509_REQ_get_pubkey (req)))
        int_error ("Error getting public key from request");
    if (X509_REQ_verify (req, pubkey) != 1)
        int_error ("Error verifying signature on certificate");

    /* read in the CA certificate */
    if (!(fp = fopen (ca_file, "r")))
        int_error ("Error reading CA certificate file");
    if (!(CAcert = PEM_read_X509 (fp, NULL, NULL, NULL)))
        int_error ("Error reading CA certificate in file");
    fclose (fp);

    /* read in the CA private key */

    if (!(fp = fopen (ca_key, "r")))
        int_error ("Error reading CA private key file");
//    CApkey = x509PKey_ReadPrivateKeyFrom(std::string(CA_KEY));
    EVP_PKEY** dummyin = NULL;
    char* pw = (char*) std::string("blackwellapps").c_str();
//    if (!(CApkey = PEM_read_PrivateKey (fp, dummyin, nullptr, pw)))
    CApkey = PEM_read_PrivateKey (fp, dummyin, &password_cb, NULL);

    if (! CApkey )
        int_error ("Error reading CA private key in file");
    fclose (fp);

/* print out the subject name and subject alt name extension */
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

/* WE SHOULD NOW ASK WHETHER TO CONTINUE OR NOT */

/* create new certificate */
  if (!(cert = X509_new ()))
    int_error ("Error creating X509 object");

/* set version number for the certificate (X509v3) and the serial number */
  if (X509_set_version (cert, 2L) != 1)
    int_error ("Error settin certificate version");
  ASN1_INTEGER_set (X509_get_serialNumber (cert), serial++);

/* set issuer and subject name of the cert from the req and the CA */
  if (!(name = X509_REQ_get_subject_name (req)))
    int_error ("Error getting subject name from request");
  if (X509_set_subject_name (cert, name) != 1)
    int_error ("Error setting subject name of certificate");
  if (!(name = X509_get_subject_name (CAcert)))
    int_error ("Error getting subject name from CA certificate");
  if (X509_set_issuer_name (cert, name) != 1)
    int_error ("Error setting issuer name of certificate");

/* set public key in the certificate */
  if (X509_set_pubkey (cert, pubkey) != 1)
    int_error ("Error setting public key of the certificate");

/* set duration for the certificate */
  if (!(X509_gmtime_adj (X509_get_notBefore (cert), 0)))
    int_error ("Error setting beginning time of the certificate");
  if (!(X509_gmtime_adj (X509_get_notAfter (cert), EXPIRE_SECS)))
    int_error ("Error setting ending time of the certificate");

/* add x509v3 extensions as specified */
  X509V3_set_ctx (&ctx, CAcert, cert, NULL, NULL, 0);
  for (i = 0; i < EXT_COUNT; i++)
    {
      X509_EXTENSION *ext;
      if (!(ext = X509V3_EXT_conf (NULL, &ctx,
				   ext_ent[i].key, ext_ent[i].value)))
	{
	  fprintf (stderr, "Error on \"%s = %s\"\n",
		   ext_ent[i].key, ext_ent[i].value);
	  int_error ("Error creating X509 extension object");
	}
      if (!X509_add_ext (cert, ext, -1))
	{
	  fprintf (stderr, "Error on \"%s = %s\"\n",
		   ext_ent[i].key, ext_ent[i].value);
	  int_error ("Error adding X509 extension to certificate");
	}
      X509_EXTENSION_free (ext);
    }

/* add the subjectAltName in the request to the cert */
  if (!X509_add_ext (cert, subjAltName, -1))
    int_error ("Error adding subjectAltName to certificate");

/* sign the certificate with the CA private key */
  if (EVP_PKEY_type (CApkey->type) == EVP_PKEY_DSA)
    digest = EVP_dss1 ();
  else if (EVP_PKEY_type (CApkey->type) == EVP_PKEY_RSA)
    digest = EVP_sha1 ();
  else
    int_error ("Error checking CA private key for a valid digest");
  if (!(X509_sign (cert, CApkey, digest)))
    int_error ("Error signing certificate");

/* write the completed certificate */
  if (!(fp = fopen (cert_file, "w")))
    int_error ("Error writing to certificate file");
  if (PEM_write_X509 (fp, cert) != 1)
    int_error ("Error while writing certificate");
  fclose (fp);

  return 0;
#endif
}
