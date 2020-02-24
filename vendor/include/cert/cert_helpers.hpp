/// pulls in all helpers headers
#ifndef cert_helpers_h
#define cert_helpers_h
#
#include "constants.hpp"
#include "version_check.hpp"

#include "cert_helpers_fs.hpp"

namespace Cert {
/**
* @brief Provides helper types and functions usefull across the whole project
*/
namespace Helpers{

/**
* This function provides a means of getting the path to "a default" certificate bundle in PEM format.
* If the openssl default value provides a path that does not exist (a likely outcome post version 1.1.1d)
* see if the SSL_CERT_FILE environment variable has been set and if that points to a file that exists.
* If not get the value of CERTLIB_DEFAULT_CERT_FILE_PATH. A copy of the OS default cert file has been loaded at that
* path as path of the installtion of dependencies
*
 */
inline static std::string replace_openssl_get_default_cert_file() {
    auto df = X509_get_default_cert_file();
    if (boost::filesystem::exists(df)) {
        return df;
    }
    auto env = X509_get_default_cert_dir_env();
    auto df_env = std::getenv(env);
    if ((df_env != nullptr) && (boost::filesystem::exists(df_env))) {
        return df_env;
    }
    return std::string(CERTLIB_DEFAULT_CERT_FILE_PATH);
}
}
}

#endif 
