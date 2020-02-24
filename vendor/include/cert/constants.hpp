#ifndef cert_constants_include_hpp
#define cert_constants_include_hpp
#include <openssl/opensslconf.h>
#define CERTLIB_DEFAULT_CERT_FILE_PATH "/usr/local/etc/openssl@1.1/cert.pem" //OPENSSLDIR/cert.pem
#undef CERTIFICAT_IMPL
#undef EVPPKEY_IMPL
#include "macros.hpp"
#endif
