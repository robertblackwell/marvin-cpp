//
//  x509_conf.cpp
//  openssl_10_6
//
//  Created by ROBERT BLACKWELL on 10/26/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "x509_conf.hpp"
#include "x509_error.hpp"

#ifdef SASASA
CONF* x509Conf_Load(std::string fileName)
{
    CONF *conf;
    long err;

    conf = NCONF_new (NCONF_default ());
    /**
    ** load the cnf file
    **/
    if (!NCONF_load (conf, fileName.c_str(), &err))
    {
        if (err == 0) {
            X509_TRIGGER_ERROR("Error opening configuration file");
        } else {
            X509_TRIGGER_ERROR("Errors parsing configuration file");
        }
    }
    return conf;
}

std::string x509Conf_GetSectionString(CONF* conf, std::string section, std::string key)
{
    char* s_val;
    const char* c_key = key.c_str();
    const char* c_section = section.c_str();
    /**
    ** Get the value of the variable PARAM(Params) that is inside the section names SEC_NAME(SectionName)
    **
    */
    if (!(s_val = NCONF_get_string (conf, c_key, c_section))) {
        X509_TRIGGER_ERROR("Error finding string");
    }
    std::string value(s_val);
    return value;
}


std::map<std::string, std::string>
x509Conf_GetAllSectionStringValues(CONF* conf, std::string section)
{
    const char* c_section = section.c_str();
    STACK_OF (CONF_VALUE) * sec;
    CONF_VALUE *item;
    int i;
    std::map<std::string, std::string> res;

    if (!(sec = NCONF_get_section (conf, c_section))) {
//    fprintf (stderr, "Error finding [%s]\n", key);
        X509_TRIGGER_ERROR("Error finding section: " + section);
    }

    /**
    ** Then iterate through all those key/value pairs and print them out
    */
    for (i = 0; i < sk_CONF_VALUE_num (sec); i++) {
        item = sk_CONF_VALUE_value (sec, i);
        std::string k(item->name);
        std::string v(item->value);
        res[k] = v;
    }
    return res;
}
#endif
//class x509Conf
//{
//    public :
//        // the ONLY way to make one is to load from a file
//        x509Conf(std::string filename);
//        ~x509Conf();
//    
//        x509Conf() = delete;
//        x509Conf(x509Conf&& other) = delete;
//        x509Conf& operator=(x509Conf& other) = delete;
//        x509Conf(const x509Conf&) = delete;
//        x509Conf& operator=(x509Conf const&) = delete;
//
//        int getGlobalNumber(std::string key);
//        std::string getGlobalString(std::string key);
//        std::string getSectionString(std::string section, std::string key);
//        std::map<std::string, std::string> getAllSectionStringValues(std::string section);
//
//
//    private:
//        CONF* _conf;
//};
x509Conf::x509Conf(std::string filename)
{
    long err;

    _conf = NCONF_new (NCONF_default ());
    /**
    ** load the cnf file
    **/
    if (!NCONF_load (_conf, filename.c_str(), &err))
    {
        if (err == 0) {
            X509_TRIGGER_ERROR("Error opening configuration file");
        } else {
            X509_TRIGGER_ERROR("Errors parsing configuration file");
        }
    }
}
x509Conf::~x509Conf()
{
    NCONF_free(_conf);
}
std::string x509Conf::getGlobalString(std::string key)
{
    const char* c_key = key.c_str();
    char* s_val;
    /**
    ** get the value (string) of the variable "GlobalVar" - example of how to find the string value
    ** of a parameter that is NOT inside a section
    **/
    if (!(s_val = NCONF_get_string (_conf, NULL, c_key))) {
        X509_TRIGGER_ERROR("Error finding string");
    }
    std::string value(s_val);
    return value;
}
int x509Conf::getGlobalNumber(std::string key)
{
    const char* c_key = key.c_str();
    long i_val;
    long err;
    
    /**
    ** Get the value (number) of variable GlobalNum from the global section - depends on version
    **/
#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
    if (!(err = NCONF_get_number_e (_conf, NULL, c_key, &i_val))) {
        X509_TRIGGER_ERROR("Error finding number");
    }
#else
    if (!(s_val = NCONF_get_string (_conf, NULL, c_key))) {
        X509_TRIGGER_ERROR("Error finding number");
    }
    i_val = atoi (s_val);
#endif
    return (int)i_val;
}

std::string x509Conf::getSectionString(std::string section, std::string key)
{
    char* s_val;
    const char* c_key = key.c_str();
    const char* c_section = section.c_str();
    /**
    ** Get the value of the variable PARAM(Params) that is inside the section names SEC_NAME(SectionName)
    **
    */
    if (!(s_val = NCONF_get_string (_conf, c_key, c_section))) {
        X509_TRIGGER_ERROR("Error finding string");
    }
    std::string value(s_val);
    return value;
}


std::map<std::string, std::string>
x509Conf::getAllSectionStringValues(std::string section)
{
    const char* c_section = section.c_str();
    const char* value;
    STACK_OF (CONF_VALUE) * sec;
    CONF_VALUE *item;
    int i;
    std::map<std::string, std::string> res;

    if (!(sec = NCONF_get_section (_conf, c_section))) {
//    fprintf (stderr, "Error finding [%s]\n", key);
        X509_TRIGGER_ERROR("Error finding section: " + section);
    }

    /**
    ** Then iterate through all those key/value pairs and print them out
    */
    for (i = 0; i < sk_CONF_VALUE_num (sec); i++) {
        item = sk_CONF_VALUE_value (sec, i);
        std::string k(item->name);
        std::string v(item->value);
        res[k] = v;
    }
    return res;
}
