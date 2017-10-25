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
  exit (-1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)

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

int password_cb(char *buf, int size, int rwflag, void *u)
{
    std::string s("secret");
    const char* c_s = s.c_str();
    std::strcpy(buf, c_s);
    return (int)s.length();
}
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
    int i;
    X509_REQ *req;
    X509_NAME *subj;
    EVP_PKEY *pkey;
    EVP_MD *digest;
    FILE *fp;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
//    seed_prng (); - dont need this on a osx

    RSA* rsaKey = RSA_generate_key(1024, RSA_3, NULL, NULL);
    EVP_PKEY* tmp_pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(tmp_pkey, rsaKey);
    pkey = tmp_pkey;

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
        int_error ("Error creating stdout BIO");

    int subjAltName_pos;
    X509_NAME *name;
    X509_EXTENSION *subjAltName;
    STACK_OF (X509_EXTENSION) * req_exts;

    /* print out the subject name and subject alt name extension */
    if (!(name = X509_REQ_get_subject_name (req)))
        int_error ("Error getting subject name from request");
    X509_NAME_print (out, name, 0);
    fputc ('\n', stdout);
    if (!(req_exts = X509_REQ_get_extensions (req)))
        int_error ("Error getting the request's extensions");
    subjAltName_pos = X509v3_get_ext_by_NID (req_exts, OBJ_sn2nid ("subjectAltName"), -1);
    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
    X509V3_EXT_print (out, subjAltName, 0, 0);
    fputc ('\n', stdout);



    /* write the completed request */
    if (!(fp = fopen (req_file.c_str(), "w")))
        int_error ("Error openning request file for write");
    if (PEM_write_X509_REQ (fp, req) != 1)
        int_error ("Error while writing request");
    fclose (fp);
    if (!(fp = fopen(pkey_file.c_str(), "w")))
        int_error ("Error openning to key file for write");
    if ( ! PEM_write_PrivateKey(
        fp,
        pkey,
        EVP_aes_128_cbc(),
        (unsigned char*) pkeyPassphrase.c_str(),
        (int)pkeyPassphrase.length(),
        password_cb, NULL)) {
        int_error("Error writing to key file");
    }
    fclose (fp);

    EVP_PKEY_free (pkey);
    X509_REQ_free (req);
    return 0;
}
