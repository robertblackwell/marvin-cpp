//
//  error.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//


#include <sstream>
#include <openssl/bio.h>
#include <exception>
#include <openssl/err.h>

#include "error.hpp"
/**
* Erro handler for Cert functions
*/
void Helpers::errorHandler (std::string func, std::string file, int lineno, std::string msg)
{
    std::string message(msg);
    std::stringstream messageStream;
    messageStream <<  "Error in function: " << func << " file: " << file << " at lineNo: " << lineno << std::endl << "Message: [" << message << "]" ;
    throw Helpers::Exception(messageStream.str());
}

using namespace Helpers;
    Helpers::Exception::Exception(std::string aMessage) : errMessage(aMessage){}
    const char* Helpers::Exception::what() const noexcept{ return errMessage.c_str(); }


