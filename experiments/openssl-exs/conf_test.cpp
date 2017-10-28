#include <cstdlib>
#include <iostream>
#include <map>
#include <openssl/conf.h>
#include "x509_error.hpp"
#include "x509_conf.hpp"
//
// example of how to interact with openssl cnf file.
//
// Treats a conf file as a database demonstrates getting a single value as a string or number
// for a variable whether that variable is inside a section or not.
//
// Further demonstrates getting all the jkey/value pairs in a section as a STACK(CONF_VALUE)
//

#define GLOB_VAR "GlobalVar"
#define GLOB_NUM "GlobalNum"
#define PARAMS "Params"
#define SEC_NAME "SectionName"

#define CONFFILE "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/openssl-exs/fixtures/test_1.cnf"

int main (int argc, char *argv[])
{
    OpenSSL_add_all_algorithms ();
    ERR_load_crypto_strings ();
    ERR_load_BIO_strings();
    ERR_load_ERR_strings();

    std::string filename(CONFFILE);
    x509Conf    conf(filename);
    std::string res = conf.getGlobalString(std::string(GLOB_VAR) );
    std::cout << "Global Var : " << res << std::endl;

    res = conf.getGlobalString(std::string(GLOB_NUM) );
    std::cout << "Global NUm : " << res << std::endl;

    res = conf.getSectionString(SEC_NAME, PARAMS);
    std::cout << "Section::variable: (" << SEC_NAME << ":" << PARAMS << ")" << res << std::endl;

    auto m = conf.getAllSectionStringValues(res);
    
    for(auto & ent : m) {
        std::cout << "m[" << ent.first << "] = " << ent.second << std::endl;
    }


#ifdef TYTYT
    conf = x509Conf_Load(filename);
    std::string res = x509Conf_GetGlobalString(conf, std::string(GLOB_VAR) );
    std::cout << "Global Var : " << res << std::endl;

    res = x509Conf_GetGlobalString(conf, std::string(GLOB_NUM) );
    std::cout << "Global NUm : " << res << std::endl;

    res = x509Conf_GetSectionString(conf, SEC_NAME, PARAMS);
    std::cout << "Section::variable: (" << SEC_NAME << ":" << PARAMS << ")" << res << std::endl;

    auto m = x509Conf_GetAllSectionStringValues(conf, res);
    
    for(auto & ent : m) {
        std::cout << "m[" << ent.first << "] = " << ent.second << std::endl;
    }
#endif
    return 0;
}
