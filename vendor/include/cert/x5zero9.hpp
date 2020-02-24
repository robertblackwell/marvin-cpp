#ifndef cert_x509_x509_include_hpp
#define cert_x509_x509_include_hpp

/**
 * A common header file for all x509 cpp files.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/ts.h>
#include <boost/unordered_set.hpp>

#include "version_check.hpp"
#include "constants.hpp"


#include "x509_types.hpp"
#include "x509_nid.hpp"

#include "x509_cert.hpp"
#include "x509_error.hpp"
#include "x509_pkey.hpp"
#include "x509_ext.hpp"
#include "x509_name.hpp"
#include "x509_serial.hpp"
#include "x509_time.hpp"
#include "x509_conf.hpp"
#include "x509_req.hpp"
#include "x509_identity.hpp"


#endif /* x509_pkey_hpp */
