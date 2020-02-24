/// pulls in all x509 headers
#ifndef cert_x509_include_hpp
#define cert_x509_include_hpp
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

#include "constants.hpp"
#include "version_check.hpp"

#include "x509_types.hpp"
#include "x509_nid.hpp"
#include "bio_utes.hpp"
#include "x509_cert.hpp"
/**
* \note - x5090/x509_cert_impl.hpp is not included - this keeps some of the messier implementation functions
* hidden from library clients. However some of the test code imprts the private header
*/
#include "x509_chain.hpp"
#include "x509_error.hpp"
#include "x509_pkey.hpp"
#include "x509_ext.hpp"
#include "x509_name.hpp"
#include "x509_serial.hpp"
#include "x509_time.hpp"
#include "x509_conf.hpp"
#include "x509_req.hpp"
#include "x509_create.hpp"

#endif
