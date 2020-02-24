/**
* Check that client code is building against a suitable boost and openssl library
*/
#ifndef cert_version_check_h
#define cert_version_check_h
#include <openssl/opensslv.h>

#define EXPECTED_OPENSSL_VERSION 0x1010104fL
#define EXPECTED_BOOST_VERSION 107200

#define HSTR(s) #s
#define XSTR(s) HSTR(s)

#define AAA 1234

#if OPENSSL_VERSION_NUMBER < EXPECTED_OPENSSL_VERSION
    #error BAD OpenSSL Version 

#elif OPENSSL_VERSION_NUMBER > EXPECTED_OPENSSL_VERSION
    #pragma message ("warning possibly an incompatible OpenSSL Version")
    #pragma message ("OPENSSL_VERSION_NUMBER: " XSTR(OPENSSL_VERSION_NUMBER) " EXPECTED_OPENSSL_VERSION: " EXPECTED_OPENSSL_VERSION)
#endif

#include <boost/version.hpp>
#if BOOST_VERSION > EXPECTED_BOOST_VERSION
    #pragma message ("warning possibly an incompatible Boost Version EXPECTED_BOOST_VERSION :"  XSTR(EXPECTED_BOOST_VERSION) " BOOST_VERSION: " XSTR(BOOST_VERSION))
#elif BOOST_VERSION < EXPECTED_BOOST_VERSION
    #error BAD Boost Version
#endif


#endif /* version_check_h */
