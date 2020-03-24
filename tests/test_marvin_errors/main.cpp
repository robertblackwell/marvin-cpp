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
// #include <marvin/error/test_error.hpp>
typedef boost::system::error_code NError;

void print_error(boost::system::error_code er){
    std::cout << er.value() << '\n';
    std::cout << er.category().name() << '\n';
    std::cout << er.category().message(er.value()) << '\n';
    
}

void xprint_error(Marvin::ErrorType er){
    std::cout << er.value() << '\n';
    std::cout << er.category().name() << '\n';
    std::cout << er.category().message(er.value()) << '\n';
    
}
void testBoosterc(boost::system::error_code& ec)
{
    std::cout << ec.value() << '\n';
    std::cout << ec.category().name() << '\n';
    std::cout << ec.category().message(ec.value()) << '\n';
}
void testMarvinerc(Marvin::ErrorType& ec)
{
    std::cout << ec.value() << '\n';
    std::cout << ec.category().name() << '\n';
    std::cout << ec.category().message(ec.value()) << '\n';
}
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
#if 0
TEST_CASE("errors")
{
    // auto te2 = Marvin::Error::make_error_code(boost::system::marvin_error::end_of_message);
    // auto te3 = Marvin::Error::make_error_code(Marvin::Error::marvin_error::error_2);
    // std::cout << te2.value() << '\n';
    // std::cout << te2.category().name() << '\n';
    // std::cout << te2.category().message(te2.value()) << '\n';
    // std::cout << te3.value() << '\n';
    // std::cout << te3.category().name() << '\n';
    // std::cout << te3.category().message(te3.value()) << '\n';

    boost::system::error_code ec = boost::system::errc::make_error_code(boost::system::errc::not_a_socket);
    boost::system::error_code el{123, boost::system::generic_category()};
    
    
    Marvin::ErrorType mec = Marvin::make_error_eob();
    // demonstrate interchangeability of the error types
    testBoosterc(mec);


    // using marvin custom make_error_code funtion
    boost::system::error_code ec3 = Marvin::make_error_code(Marvin::errc::end_of_message);
    std::cout << ec3.value() << '\n';
    std::cout << ec3.category().name() << '\n';
    std::cout << ec3.category().message(ec3.value()) << '\n';
    
    // assigning marvin error codes to a boost error code variable
    // and assigning eof to boost::system::error_code
    boost::system::error_code erc = Marvin::errc::end_of_message;
    boost::system::error_code erc2 = boost::asio::error::misc_errors::eof;
    auto erc3 = boost::system::error_code(boost::system::errc::success, boost::system::system_category());
    auto mrc3 = erc3.message();
    
    auto e1 = (Marvin::make_error_ok());
    auto m1 = e1.message();
    bool flg = (!e1);
    auto e2 = (Marvin::make_error_eof());
    auto m2 = e2.message();
    auto e3 = (Marvin::make_error_eom());
    auto m3 = e3.message();

    print_error(Marvin::make_error_ok());
    print_error(Marvin::make_error_eof());
    print_error(Marvin::make_error_eom());

    print_error(Marvin::Error::make_ok());
    print_error(Marvin::Error::make_eof());
    print_error(Marvin::Error::make_eom());
}
#endif