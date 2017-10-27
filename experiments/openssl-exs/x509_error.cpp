//
//  x509_error.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_error.hpp"
#include <sstream>
#include <openssl/bio.h>
#include <exception>

void x509Error_Handler (std::string file, int lineno, std::string msg)
//void x509Error_Handler (const char *file, int lineno, const char *msg)
{
    char buf[100001];
    std::string message(msg);
    std::stringstream messageStream;
    
    messageStream <<  "Error in file: " << file << " at lineNo: " << lineno << std::endl << "Message: " << message << std::endl;
    
    BIO* errBio = BIO_new(BIO_s_mem());
    ERR_print_errors(errBio);

    int count2 = BIO_read(errBio, buf, 10000);
    buf[count2] = (char)0;
    messageStream << "OpenSSL ERR: " << buf << std::endl;
//    ERR_print_errors_fp (stderr);
//    std::cout << messageStream.str();
//    throw std::runtime_error(messageStream.str());
    throw x509Exception(messageStream.str());
//  exit (-1);
}

x509Exception::x509Exception(std::string aMessage) : x509_ErrMessage(aMessage){}
const char* x509Exception::what() const noexcept{ return x509_ErrMessage.c_str(); }


