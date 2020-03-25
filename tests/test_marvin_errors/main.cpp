//
//  main.cpp
//  error_test
//
//  Created by ROBERT BLACKWELL on 12/5/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <string>
#include <iostream>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
TEST_CASE("make_marvin_errors")
{
    Marvin::ErrorType meob = Marvin::make_error_eob();
    auto meobv = meob.value();
    CHECK(meob.value() == static_cast<int>(Marvin::errc::end_of_body));
    CHECK(meob == Marvin::make_error_eob());

    Marvin::ErrorType meom = Marvin::make_error_eom();
    auto meomv = meom.value();
    CHECK(meom.value() == static_cast<int>(Marvin::errc::end_of_message));
    CHECK(meom == Marvin::make_error_eom());

    Marvin::ErrorType meop = Marvin::make_error_parse();
    auto meopv = meop.value();
    CHECK(meop.value() == static_cast<int>(Marvin::errc::parser_error));
    CHECK(meop == Marvin::make_error_parse());

    Marvin::ErrorType mok = Marvin::make_error_ok();
    auto mokv = mok.value();
    CHECK(mok.value() == static_cast<int>(Marvin::errc::ok));
    CHECK(mok == Marvin::make_error_ok());

}
