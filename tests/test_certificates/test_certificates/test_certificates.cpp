//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <pthread.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>

#include <marvin/certificates/certificates.hpp>
#include <marvin/certificates/env_utils.hpp>

TEST_CASE("certificates")
{
    Marvin::Certificates certificates = Marvin::Certificates::getInstance();
    CHECK(certificates.get_X509_STORE_ptr() != nullptr);
}
