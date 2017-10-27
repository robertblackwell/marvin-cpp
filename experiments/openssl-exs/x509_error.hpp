//
//  x509_error.hpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef x509_error_hpp
#define x509_error_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <exception>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

//#define int_error(msg) handle_error(__FILE__, __LINE__, msg)

#define X509_TRIGGER_ERROR(msg) x509Error_Handler(__FILE__, __LINE__, msg)

//void x509Error_Handler (const char *file, int lineno, const char *msg);
void x509Error_Handler (std::string file, int lineno, std::string msg);

class x509Exception : public std::exception
{
    public:
        x509Exception(std::string message);
        const char* what() const noexcept;
    private:
        std::string x509_ErrMessage;
};


#endif /* x509_error_hpp */
