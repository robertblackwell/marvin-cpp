#ifndef marvin_unit_test_hpp
#define marvin_unit_test_hpp
#define MARVIN_CATCHX
#ifdef MARVIN_CATCH 
    #include <catch2/catch.hpp>
    #define MARVIN_CONFIG_RUNNER CATCH_CONFIG_RUNNER
#else
    #include <doctest/doctest.h>
    #define MARVIN_CONFIG_RUNNER  DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN CATCH_CONFIG_RUNNER
#endif

#endif
