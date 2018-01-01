#ifndef marvin_tests_runners_hpp
#define marvin_tests_runners_hpp
#include <gtest/gtest.h>
void startTestServer(long port);
void stopTestServer();

void startProxyServer(long port);
void stopProxyServer();
#endif
