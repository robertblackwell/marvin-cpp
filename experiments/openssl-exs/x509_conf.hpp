//
//  x509_conf.hpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef x509_conf_hpp
#define x509_conf_hpp

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
/*
* x509Conf - a wrapper class around an openssl CONF* - which holds the contents of an
* openssl .cnf file.
*
* Member functions allow access to variable values within the loaded inage of a .cnf file
*
*/
class x509Conf
{
    public :
        // the ONLY way to make one is to load from a file. Cannnot copy or move one of these
        x509Conf(std::string filename);
        ~x509Conf();
    
        x509Conf() = delete;
        x509Conf(x509Conf&& other) = delete;
        x509Conf& operator=(x509Conf& other) = delete;
        x509Conf(const x509Conf&) = delete;
        x509Conf& operator=(x509Conf const&) = delete;

        int         getGlobalNumber(std::string key);
        std::string getGlobalString(std::string key);
        std::string getSectionString(std::string section, std::string key);
        std::map<std::string, std::string> getAllSectionStringValues(std::string section);


    private:
        CONF* _conf;
};

#endif /* x509_conf_hpp */
