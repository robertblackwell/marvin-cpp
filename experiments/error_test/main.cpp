//
//  main.cpp
//  error_test
//
//  Created by ROBERT BLACKWELL on 12/5/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include "marvin_error.hpp"

typedef boost::system::error_code NError;

class FError{
    public:
    static NError ok(){
        boost::system::error_code r = boost::system::errc::make_error_code(boost::system::errc::success);
        return r;
    };
    static NError eof(){
        boost::system::error_code r = boost::asio::error::misc_errors::eof;
        return r;
    };
    static NError eom(){
        boost::system::error_code r = Marvin::errc::end_of_message;
        return r;
    };
};

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

int main()
{

    Marvin::ErrorType mec = Marvin::make_error_eob();
    boost::system::error_code bec = make_error_code(boost::system::errc::stream_timeout);
    // demonstrate interchangeability of the error types
    testBoosterc(mec);
    testMarvinerc(bec);


    // using marvin custom make_error_code funtion
    boost::system::error_code ec3 = Marvin::make_error_code(Marvin::errc::end_of_message);
    std::cout << ec3.value() << '\n';
    std::cout << ec3.category().name() << '\n';
    std::cout << ec3.category().message(ec3.value()) << '\n';
    
    // assigning marvin error codes to a boost error code variable
    // and assigning eof to boost::system::error_code
    boost::system::error_code erc = Marvin::errc::end_of_message;
    boost::system::error_code erc2 = boost::asio::error::misc_errors::eof;
    
    print_error(FError::ok());
    print_error(FError::eof());
    print_error(FError::eom());

    print_error(Marvin::make_error_ok());
    print_error(Marvin::make_error_eof());
    print_error(Marvin::make_error_eom());

    print_error(Marvin::Error::make_ok());
    print_error(Marvin::Error::make_eof());
    print_error(Marvin::Error::make_eom());
    
    
    return 0;
}
