//
//  x509_extension.cpp
//  req_test
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include "x509_error.hpp"
#include "x509_extension.hpp"

ExtensionStack x509ExtensionStack_New()
{
    ExtensionStack extlist = sk_X509_EXTENSION_new_null ();
    return extlist;
}
void x509ExtensionStack_Free(ExtensionStack stk)
{
//    sk_X509_EXTENSION_free(stk);
    sk_X509_EXTENSION_pop_free (stk, X509_EXTENSION_free);

}

void x509ExtensionStack_AddByNID(STACK_OF(X509_EXTENSION) *sk, int nid, std::string value)
{
    X509_EXTENSION *ex;
    ex = X509V3_EXT_conf_nid(NULL, NULL, nid, (char*)value.c_str());
    if (!ex)
        X509_TRIGGER_ERROR("could not create extensions with NID");
    sk_X509_EXTENSION_push(sk, ex);
}

void x509ExtensionStack_AddBySN(ExtensionStack stack, std::string sn_string, std::string value)
{
    char* sn_name_c = (char*)sn_string.c_str();
    char* value_c = (char*) value.c_str();
    X509_EXTENSION* ext;
    
    if (!(ext = X509V3_EXT_conf (NULL, NULL, sn_name_c, value_c)))
        X509_TRIGGER_ERROR("Error creating extension name : " + sn_string + " value: " + value);

    sk_X509_EXTENSION_push (stack, ext);
}
