/// \brief this file (and its ccp) implement tests aim at verifying the correct
/// operation of the man-in-the-middle proxy.
///
/// These tests send correctly formated
/// proxy request to the mitm proxy, these should be forwarded on our test server
/// verified and returned where they are again verified.
///
///
#ifndef marvin_tp_proxy_tests_hpp
#define marvin_tp_proxy_tests_hpp
#include "message.hpp"
#include "tp_testcase.hpp"

std::vector<tp::TestcaseSPtr> makeWhiteacornTestcases();
std::vector<tp::TestcaseSPtr> makeTestServerTestcases();

#endif /* tp_proxt_tests_hpp */
