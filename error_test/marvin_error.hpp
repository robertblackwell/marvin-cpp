//
//  main.cpp
//  error_test
//
//  Created by ROBERT BLACKWELL on 12/5/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#ifndef MARVIN_ERROR_HPP
#define MARVIN_ERROR_HPP
#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>

/**
 * This file defines a few Marvin specific error codes based on boost::system::error_code
 *
 *  To save typing I have typdef'd boost::system::error_code as Marvin::ErrorType
 *  and provided utility functions for creating a small set of standard and custom
 *  error codes
 */

namespace Marvin{
    
    typedef boost::system::error_code ErrorType;

    enum class errc {
        end_of_message = 21
    };

    class category : public boost::system::error_category
    {
    public:
        const char *name() const noexcept { return "Marvin error"; }
        std::string message(int ev) const {
            Marvin::errc e = static_cast<Marvin::errc>(ev);
            switch( e ){
                case Marvin::errc::end_of_message :
                    return "end of message";
                    break;
                default:
                    return "unknow Marvin error";
                    break;
            }
            return "error message ";
        }
    };


    static Marvin::category cat;
    
    const boost::system::error_category& category(){
        return cat;
    }
    boost::system::error_code make_error_code(Marvin::errc e)
    {
        return boost::system::error_code(static_cast<int>(e),cat);
    }
    boost::system::error_condition make_error_condition(Marvin::errc e)
    {
        return boost::system::error_condition(static_cast<int>(e), cat);
    }
    
} // namespace Marvin

namespace boost {
namespace system {
        template <>
        struct is_error_code_enum<Marvin::errc>
        : public std::true_type {};
}} // namespace boost::system

namespace Marvin{
    
    ErrorType make_error_ok(){
        boost::system::error_code r = boost::system::errc::make_error_code(boost::system::errc::success);
        return r;
    }
    ErrorType make_error_eof(){
        boost::system::error_code r = boost::asio::error::misc_errors::eof;
        return r;
    };
    ErrorType make_error_eom(){
        boost::system::error_code r = Marvin::errc::end_of_message;
        return r;
    };
} // namespace Marvin

namespace Marvin{
namespace Error{
        ErrorType  make_ok(){ return Marvin::make_error_ok();}
        ErrorType  make_eof(){ return Marvin::make_error_eof();};
        ErrorType  make_eom(){ return Marvin::make_error_eom();};
}} // namespace Marvin::Error


#endif
