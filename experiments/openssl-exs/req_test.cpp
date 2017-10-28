#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "x509_error.hpp"
#include "x509_pkey.hpp"
#include "x509_req.hpp"
#include "x509_extension.hpp"


#define PKEY_FILE "privkey.pem"
#define REQ_FILE "newreq.pem"
#define ENTRY_COUNT 6

const std::string fixtures_directory = "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/openssl-exs/fixtures";
const std::string pkey_file = fixtures_directory + "/" + "ex5-pkey.pem";
const std::string req_file = fixtures_directory + "/" + "ex5-req.pem";
const std::string pkeyPassphrase("blackwellapps");
struct entry
{
  char *key;
  char *value;
};

typedef struct key_value {
    std::string key;
    std::string value;
} KeyValue;

KeyValue makeEntry(std::string key, std::string value)
{
    KeyValue ent;
    ent.key = key;
    ent.value = value;
    return ent;
}
std::map<std::string, std::string> subjectValues = {
  {"countryName", "US"},
  {"stateOrProvinceName", "VA"},
  {"localityName", "Fairfax"},
  {"organizationName", "Zork.org"},
  {"organizationalUnitName", "Server Division"},
  {"commonName", "Server 36, Engineering"},
};

std::vector<std::string> subjectKeys = {
  "countryName",
  "stateOrProvinceName",
  "localityName",
  "organizationName",
  "organizationalUnitName",
  "commonName",
};

KeyValue keyValues[] = {
  {"countryName", "US"},
  {"stateOrProvinceName", "VA"},
  {"localityName", "Fairfax"},
  {"organizationName", "Zork.org"},
  {"organizationalUnitName", "Server Division"},
  {"commonName", "Server 36, Engineering"},
};

struct entry entries[ENTRY_COUNT] = {
  {"countryName", "US"},
  {"stateOrProvinceName", "VA"},
  {"localityName", "Fairfax"},
  {"organizationName", "Zork.org"},
  {"organizationalUnitName", "Server Division"},
  {"commonName", "Server 36, Engineering"},
};

/* Add extension using V3 code: we can set the config file as NULL
 * because we wont reference any other sections.
 */

int add_ext(STACK_OF(X509_EXTENSION) *sk, int nid, std::string value)
{
    X509_EXTENSION *ex;
    ex = X509V3_EXT_conf_nid(NULL, NULL, nid, (char*)value.c_str());
    if (!ex)
        return 0;
    sk_X509_EXTENSION_push(sk, ex);

    return 1;
}


int
main (int argc, char *argv[])
{
    X509_REQ *req;
    EVP_PKEY *pkey;
    EVP_MD *digest = nullptr;
    std::string password("blackwellapps");

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    ERR_load_BIO_strings();
    ERR_load_ERR_strings();

    pkey = x509Rsa_Generate();
    req = x509Req_New();
    x509Req_SetPublicKey(req, pkey);
    x509Req_SetSubjectName(req, subjectValues);
    ExtensionStack stk = x509ExtensionStack_New();

#ifdef SN_EXT
    x509ExtensionStack_AddBySN(stk, SN_subject_alt_name, std::string("DNS:another.blackwell.com,DNS:another2.blackwell.com") );
#else
    x509ExtensionStack_AddByNID(stk, NID_subject_alt_name, std::string("DNS:another.blackwell.com,DNS:another2.blackwell.com"));
#endif

    x508Req_AddExtensions(req, stk);

    /*
    ** pick the correct digest and sign the request
    */
    if (EVP_PKEY_type (pkey->type) == EVP_PKEY_DSA)
        // this cast is for a, thankfully, pedantic C++ compiler
        digest = (EVP_MD*)EVP_dss1 ();
    else if (EVP_PKEY_type (pkey->type) == EVP_PKEY_RSA)
        // this cast is for a, thankfully, pedantic C++ compiler
        digest = (EVP_MD*)EVP_sha256();
    else
        X509_TRIGGER_ERROR("Error checking public key for a valid digest");
   
    x509Req_Sign(req, pkey, digest);
    x509Req_WriteToFile(req, req_file);
    x509PKey_WritePrivateKey(pkey,  pkey_file, password);

#ifdef TTTTTT
//    RSA* rsaKey = RSA_generate_key(1024, RSA_3, NULL, NULL);
//    EVP_PKEY* tmp_pkey = EVP_PKEY_new();
//    EVP_PKEY_assign_RSA(tmp_pkey, rsaKey);
//    pkey = tmp_pkey;

#ifdef PKEY_EXTERNAL
    pem_password_cb* pcb = NULL;
    /* first read in the private key */
    if (!(fp = fopen (PKEY_FILE, "r")))
        int_error ("Error reading private key file");
    if (!(pkey = PEM_read_PrivateKey (fp, NULL, password_cb, NULL)))
        int_error ("Error reading private key in file");
    fclose (fp);
#endif

    /* create a new request and add the key to it */
    if (!(req = X509_REQ_new ()))
        int_error ("Failed to create X509_REQ object");
    X509_REQ_set_pubkey (req, pkey);

    /*
    * assign the subject names - there are ENTRY_COUNT parts to this name
    */
    if (!(subj = X509_NAME_new ()))
        int_error ("Failed to create X509_NAME object");
    /*
    ** Loop over the elements of the subjectName and build up a compound CN as a single
    ** value
    */
    for (auto const& key : subjectKeys) {
        int nid;
        X509_NAME_ENTRY *ent;
        if ( subjectValues.find(key) == subjectValues.end() ) {
            std::cout << key << " not found " << std::endl;
        } else {
//            std::cout << key << " found " << std::endl;
            if ((nid = OBJ_txt2nid (key.c_str())) == NID_undef) {
                fprintf (stderr, "Error finding NID for %s\n", key.c_str());
                int_error ("Error on lookup");
            }
//            std::cout << key << " nid found " << nid<<std::endl;
            unsigned char* v = (unsigned char*)subjectValues[key].c_str();
            
            if (!(ent = X509_NAME_ENTRY_create_by_NID ((X509_NAME_ENTRY**)NULL, nid, MBSTRING_ASC, v, -1)))
                int_error ("Error creating Name entry from NID");
            
            if (X509_NAME_add_entry (subj, ent, -1, 0) != 1)
                int_error ("Error adding entry to Name");
        }
    }
    if (X509_REQ_set_subject_name (req, subj) != 1)
        int_error ("Error adding subject to request");

    /*
    ** add an extension for the FQDN we wish to have
    */
    {
    X509_EXTENSION *ext;
    X509_EXTENSION *ext2;
    STACK_OF (X509_EXTENSION) * extlist;
    char *name = (char*) std::string("subjectAltName").c_str();
    char *value = (char*) std::string("DNS:splat.zork.org").c_str();
    char *value2 = (char*) std::string("DNS:2222.splat.zork.org").c_str();

    extlist = sk_X509_EXTENSION_new_null ();

    if (!(ext = X509V3_EXT_conf (NULL, NULL, name, value)))
        int_error ("Error creating subjectAltName extension");

    if (!(ext2 = X509V3_EXT_conf (NULL, NULL, name, value2)))
        int_error ("Error creating subjectAltName extension");

    sk_X509_EXTENSION_push (extlist, ext);
    sk_X509_EXTENSION_push (extlist, ext2);
    

    /* Some Netscape specific extensions */
    add_ext(extlist, NID_netscape_cert_type, "client,email");
    add_ext(extlist, NID_subject_alt_name, "email:steve@openssl.org");
    add_ext(extlist, NID_subject_alt_name, "email:one.steve@openssl.org");
    add_ext(extlist, NID_subject_alt_name, "email:two.steve@openssl.org");

    if (!X509_REQ_add_extensions (req, extlist))
        int_error ("Error adding subjectAltName to the request");
    sk_X509_EXTENSION_pop_free (extlist, X509_EXTENSION_free);
    }

    /*
    ** pick the correct digest and sign the request
    */
    if (EVP_PKEY_type (pkey->type) == EVP_PKEY_DSA)
        // this cast is for a pedantic C++ compiler
        digest = (EVP_MD*)EVP_dss1 ();
    else if (EVP_PKEY_type (pkey->type) == EVP_PKEY_RSA)
        // this cast is for a pedantic C++ compiler
        digest = (EVP_MD*)EVP_sha256();
    else
        int_error ("Error checking public key for a valid digest");
   
    if (!(X509_REQ_sign (req, pkey, digest)))
        int_error ("Error signing request");


    BIO *out;
    /* open stdout */
    if (!(out = BIO_new_fp (stdout, BIO_NOCLOSE)))
        X509_TRIGGER_ERROR("Error creating stdout BIO");

    int subjAltName_pos;
    X509_NAME *name;
    X509_EXTENSION *subjAltName;
    STACK_OF (X509_EXTENSION) * req_exts;

    /* print out the subject name and subject alt name extension */
    if (!(name = X509_REQ_get_subject_name (req)))
        X509_TRIGGER_ERROR("Error getting subject name from request");
    X509_NAME_print (out, name, 0);
    fputc ('\n', stdout);
    if (!(req_exts = X509_REQ_get_extensions (req)))
        X509_TRIGGER_ERROR("Error getting the request's extensions");
    subjAltName_pos = X509v3_get_ext_by_NID (req_exts, OBJ_sn2nid ("subjectAltName"), -1);
    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
    X509V3_EXT_print (out, subjAltName, 0, 0);
    fputc ('\n', stdout);



    /* write the completed request */
    if (!(fp = fopen (req_file.c_str(), "w")))
        X509_TRIGGER_ERROR ("Error openning request file for write");
    if (PEM_write_X509_REQ (fp, req) != 1)
        X509_TRIGGER_ERROR("Error while writing request");
    fclose (fp);
    if (!(fp = fopen(pkey_file.c_str(), "w")))
        X509_TRIGGER_ERROR("Error openning to key file for write");
    char* pw = (char*) password.c_str();
    if ( ! PEM_write_PrivateKey(
        fp,
        pkey,
        EVP_aes_128_cbc(),
        (unsigned char*) pkeyPassphrase.c_str(),
        (int)pkeyPassphrase.length(),
        nullptr, NULL)) {
        X509_TRIGGER_ERROR("Error writing to key file");
    }
    fclose (fp);
#endif
    EVP_PKEY_free (pkey);
    X509_REQ_free (req);
    return 0;
}
