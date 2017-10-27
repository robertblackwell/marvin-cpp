//
//  x509_ext.cpp
//  all
//
//  Created by ROBERT BLACKWELL on 10/24/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_req.hpp"
#include "x509_error.hpp"
#pragma mark x509Req functions

X509_REQ* x509Req_New()
{
    X509_REQ* req;
    if (!(req = X509_REQ_new ()))
        X509_TRIGGER_ERROR("Failed to create X509_REQ object");
    return req;
}
void x509Req_Free(X509_REQ* req)
{
    X509_REQ_free (req);
}
void X508Req_AddExtensions(X509_REQ* req, ExtensionStack stack)
{
    if (!X509_REQ_add_extensions (req, stack))
        X509_TRIGGER_ERROR("Error adding subjectAltName to the request");
}

void x509Req_SetPublicKey(X509_REQ* req, EVP_PKEY* pkey )
{
    X509_REQ_set_pubkey (req, pkey);
}

void x509Req_SetSubjectName(X509_REQ* req, std::map<std::string, std::string> subjectValues)
{
    X509_NAME* subj;
    /*
    * assign the subject names - there are ENTRY_COUNT parts to this name
    */
    if (!(subj = X509_NAME_new ()))
        X509_TRIGGER_ERROR("Failed to create X509_NAME object");
    /*
    ** Loop over the elements of the subjectName and build up a compound CN as a single
    ** value
    */
    std::vector<std::string> subjectKeys = {
      "countryName",
      "stateOrProvinceName",
      "localityName",
      "organizationName",
      "organizationalUnitName",
      "commonName",
    };
    for (auto const& key : subjectKeys) {
        int nid;
        X509_NAME_ENTRY *ent;
        if ( subjectValues.find(key) == subjectValues.end() ) {
            std::cout << key << " not found " << std::endl;
        } else {
//            std::cout << key << " found " << std::endl;
            if ((nid = OBJ_txt2nid (key.c_str())) == NID_undef) {
                fprintf (stderr, "Error finding NID for %s\n", key.c_str());
                X509_TRIGGER_ERROR("Error on lookup");
            }
//            std::cout << key << " nid found " << nid<<std::endl;
            unsigned char* v = (unsigned char*)subjectValues[key].c_str();
            
            if (!(ent = X509_NAME_ENTRY_create_by_NID ((X509_NAME_ENTRY**)NULL, nid, MBSTRING_ASC, v, -1)))
                X509_TRIGGER_ERROR("Error creating Name entry from NID");
            
            if (X509_NAME_add_entry (subj, ent, -1, 0) != 1)
                X509_TRIGGER_ERROR("Error adding entry to Name");
        }
    }
    if (X509_REQ_set_subject_name (req, subj) != 1)
        X509_TRIGGER_ERROR ("Error adding subject to request");
}

void
x509Req_VerifySignature(X509_REQ* req)
{
    EVP_PKEY* pkey;
    /* verify signature on the request */
    if (!(pkey = X509_REQ_get_pubkey (req)))
        X509_TRIGGER_ERROR ("Error getting public key from request");
    if (X509_REQ_verify (req, pkey) != 1)
        X509_TRIGGER_ERROR ("Error verifying signature on certificate");
}

X509_REQ*
x509Req_ReadFromFile(std::string fileName)
{
    X509_REQ* req;
    FILE* fp;
    char* fn = (char*)fileName.c_str();
    if (!(fp = fopen (fn, "r")))
        X509_TRIGGER_ERROR ("Error reading request file");
    if (!(req = PEM_read_X509_REQ (fp, NULL, NULL, NULL)))
        X509_TRIGGER_ERROR ("Error reading request in file");
    fclose (fp);
    x509Req_VerifySignature(req);
    return req;
}

X509_NAME*
x509Req_GetSubjectName(X509_REQ* req)
{
    X509_NAME *name;
    if (!(name = X509_REQ_get_subject_name (req)))
        X509_TRIGGER_ERROR ("Error getting subject name from request");
    return name;
}
X509_EXTENSION*
x509Req_GetSubjectAltName(X509_REQ* req)
{
    X509_EXTENSION *subjAltName;
    STACK_OF (X509_EXTENSION) * req_exts;
    int subjAltName_pos;
    if (!(req_exts = X509_REQ_get_extensions (req)))
        X509_TRIGGER_ERROR ("Error getting the request's extensions");
    subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
                       OBJ_sn2nid ("subjectAltName"), -1);
    subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
    return subjAltName;
}

STACK_OF(X509_EXTENSION*) x509Req_GetExtensions(X509_REQ* req)
{
    STACK_OF(X509_EXTENSION*) stack = X509_REQ_get_extensions(req);
    return stack;
}

void x509Req_PrintNames(BIO* out, X509_REQ* req)
{
  X509_NAME *name;
  X509_EXTENSION *subjAltName;
  STACK_OF (X509_EXTENSION) * req_exts;
  int subjAltName_pos;

  if (!(name = X509_REQ_get_subject_name (req)))
    X509_TRIGGER_ERROR ("Error getting subject name from request");
  X509_NAME_print (out, name, 0);
  fputc ('\n', stdout);
  if (!(req_exts = X509_REQ_get_extensions (req)))
    X509_TRIGGER_ERROR ("Error getting the request's extensions");
  subjAltName_pos = X509v3_get_ext_by_NID (req_exts,
                       OBJ_sn2nid ("subjectAltName"), -1);
  subjAltName = X509v3_get_ext (req_exts, subjAltName_pos);
  X509V3_EXT_print (out, subjAltName, 0, 0);
  fputc ('\n', stdout);
}


