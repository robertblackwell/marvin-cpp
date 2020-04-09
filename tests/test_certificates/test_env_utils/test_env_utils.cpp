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

TEST_CASE("env-home-home")
{
    Marvin::setMarvinHomeToUserHome();
    auto xx2 = getenv("MARVIN_HOME");
    auto yy2 = getenv("HOME");
    auto x = Marvin::validEnvVariables();
    CHECK(x);
}
TEST_CASE("env-home-project-home")
{
    Marvin::setMarvinHomeToProjectHome();
    auto xx2 = getenv("MARVIN_HOME");
    auto yy2 = getenv("HOME");
    auto x = Marvin::validEnvVariables();
    CHECK(x);
}
TEST_CASE("env-home-bad")
{
    // char* marvin_home = const_cast<char*>("MARVIN_HOME");
    setenv("MARVIN_HOME", "ABADVALUE", 1);
    auto xx2 = getenv("MARVIN_HOME");
    auto yy2 = getenv("HOME");
    auto x = Marvin::validEnvVariables();
    CHECK(!x);
}

TEST_CASE("marvin pwd test - home")
{
    using namespace boost::filesystem;
    path pwd = current_path();
    path tmp_to = path("/home/robert");
    chdir(tmp_to.string().c_str());
    auto x = Marvin::validWorkingDir();
    CHECK(x);
}
TEST_CASE("marvin pwd test - marvin++")
{
    using namespace boost::filesystem;
    path pwd = current_path();
    path tmp_to = path("/home/robert/Projects/marvin++");
    chdir(tmp_to.string().c_str());
    auto x = Marvin::validWorkingDir();
    CHECK(x);
}
