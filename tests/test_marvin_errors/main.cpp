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
#include <marvin/boost_stuff.hpp>
#include <marvin/error/marvin_error.hpp>
#include "test_error.hpp"
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
#if 0
/// start defn of test_error.hpp ==========================================
class test_category : public boost::system::error_category
{
    const char* name() const noexcept;
    std::string message(int ev) const;
};
namespace boost{
namespace system{
    const error_category & test_category();
    namespace test_error {
        enum test_error_code {
            success = 0,
            error_1 = 22,
            error_2 = 33
        };
    }
    template<> struct is_error_code_enum<test_error::test_error_code>
        { static const bool value = true; };

    namespace test_error
    {
        error_code make_error_code( test_error_code e );
    }
}
}
/// headers end ===================================
/// start test_error.cpp ===========================
const char* test_category::name() const noexcept
{
//        std::string s = "test category ev";
//        const char* r = (const char*) s.c_str();
    return "test_category";
}
std::string test_category::message(int ev) const
{
    return "error message ev: " + std::to_string(ev);
}

static const test_category test_cat_const;

namespace boost {
namespace system{
    const error_category & test_category(){
//        static const class test_category test_category_const;
        return test_cat_const;
    }
}
}

namespace boost {
namespace system{
    namespace test_error {
//        enum test_error_code {
//            success = 0,
//            error_1 = 22,
//            error_2 = 33
//        };
    }
//    template<> struct is_error_code_enum<test_error::test_error_code>
//        { static const bool value = true; };

    namespace test_error
    {
        error_code make_error_code( test_error_code e ) {
            return error_code( e, test_category() );
        }
    }
} // namespace system
} // namespace boost
namespace Marvin{
namespace Error{
    boost::system::error_code make_error_code(boost::system::test_error::test_error_code e) {
        return boost::system::test_error::make_error_code(e);
    }
    using test_error = boost::system::test_error::test_error_code;
}
}
#endif
int main()
{
    auto te = boost::system::marvin_error::make_error_code(boost::system::marvin_error::ok);
    auto te2 = Marvin::Error::make_error_code(boost::system::marvin_error::end_of_message);
    auto te3 = Marvin::Error::make_error_code(Marvin::Error::marvin_error::error_2);
    std::cout << te.value() << '\n';
    std::cout << te.category().name() << '\n';
    std::cout << te.category().message(te.value()) << '\n';
    std::cout << te2.value() << '\n';
    std::cout << te2.category().name() << '\n';
    std::cout << te2.category().message(te2.value()) << '\n';
    std::cout << te3.value() << '\n';
    std::cout << te3.category().name() << '\n';
    std::cout << te3.category().message(te3.value()) << '\n';

    std::cout << Marvin::Error::error_description(te) << std::endl;
    std::cout << Marvin::Error::error_description(te2) << std::endl;
    std::cout << Marvin::Error::error_description(te3) << std::endl;

    boost::system::error_code ec = boost::system::errc::make_error_code(boost::system::errc::not_a_socket);
    boost::system::error_code el{123, boost::system::generic_category()};
    
    
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
    auto erc3 = boost::system::error_code(boost::system::errc::success, boost::system::system_category());
    auto mrc3 = erc3.message();
    
    auto e1 = (Marvin::make_error_ok());
    auto m1 = e1.message();
    bool flg = (!e1);
    auto e2 = (Marvin::make_error_eof());
    auto m2 = e2.message();
    auto e3 = (Marvin::make_error_eom());
    auto m3 = e3.message();

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
