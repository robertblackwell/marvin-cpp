//
//  x509_pkey.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_pkey.hpp"
#include "x509_error.hpp"

std::string const_password("blackwellapps");
typedef int (*PemPasswordCb)(char *buf, int size, int rwflag, void *u);

int password_cb(char *buf, int size, int rwflag, void *u)
{
    std::string s(const_password);
    const char* c_s = s.c_str();
    std::strcpy(buf, c_s);
    return (int)s.length();
}

EVP_PKEY* x509Rsa_Generate()
{
    EVP_PKEY* tmp_pkey;
    RSA* rsaKey = RSA_generate_key(1024, RSA_3, NULL, NULL);
    tmp_pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(tmp_pkey, rsaKey);
    return tmp_pkey;
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
        X509_TRIGGER_ERROR( "ReadPrivateKeyFrom:: failed to open key file");

// have not implemented the callback version of loading a private key

#define PWCB_NO
#ifdef PWCB
    if (!(pkey = PEM_read_PrivateKey (fp, dummyIn, &password_cb, NULL)))
        X509_TRIGGER_ERROR ("ReadPrivateKey::Error reading private key in file");
#else
    PemPasswordCb nullCb = nullptr;
    void* pw = (void*) password.c_str();
    if (!(pkey = PEM_read_PrivateKey (fp, dummyIn, nullCb, pw)))
        X509_TRIGGER_ERROR ("ReadPrivateKey::Error reading private key in file");
#endif
    fclose(fp);
    
    return pkey;
}
EVP_PKEY* x509PKey_ReadPrivateKeyFrom(std::string fileName)
{
    throw "not implemented";
    return x509PKey_ReadPrivateKeyFrom(fileName, const_password);
}
