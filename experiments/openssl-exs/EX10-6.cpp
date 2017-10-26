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
#include "x509_ext.hpp"
#include "CertificateAuthority.hpp"
#include "CertificateBuilder.hpp"

//
// all of these globals need to be parameters or properties
//
const std::string CA_FILE("/Users/rob/CA/private/cacert.pem");
const std::string CA_KEY("/Users/rob/CA/private/cakey.pem");
const std::string REQ_FILE("/Users/rob/CA/sites/ssltest/request.pem");
const std::string CERT_FILE("/Users/rob/CA/sites/ssltest/newcert.pem");
const std::string ORIGINAL_CERT_FILE("/Users/rob/CA/certs/www.google.com/real_certificate.pem");

std::map<std::string, std::string> extensions = {
    //
    // this demonstrates how to use "standard names" SN_xxxxxx #defines are probided by openssl
    // through the header file crypto/obj_dat.h
    //
    {std::string(SN_basic_constraints), "CA:false"},
    {std::string(SN_netscape_comment), "\"XXOpenSSL Generated Certificate\""},
    {std::string(SN_subject_key_identifier), "hash"},
    //
    // can also us arbitary strings which MAY be treated as custom if they do not exactly match
    // an already defined SN_xxxx value
    //
    {"authorityKeyIdentifier", "keyid,issuer:always"},
    {"keyUsage", "nonRepudiation,digitalSignature,keyEncipherment"}
};

static std::string const_password("blackwellapps");

int
main (int argc, char *argv[])
{
    X509* cert;
    X509* original_cert;
    X509_REQ *req;

    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    
    CertificateAuthority certAuth("/Users/rob/CA/private");
    auto builder = new CertificateBuilder(certAuth);
    req = x509Req_ReadFromFile(REQ_FILE);
    cert = builder->buildFromReq(req, extensions);

    x509Cert_WriteToFile(cert, CERT_FILE);
    

    return 0;
}
