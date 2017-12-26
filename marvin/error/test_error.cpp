//
//  marvin_error.cpp
//  marvin_connect
//
//  Created by ROBERT BLACKWELL on 12/25/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//
#include <sstream>
#include "test_error.hpp"
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

/// start marvin_error.cpp ===========================
const char* boost::system::marvin_category::name() const noexcept
{
//        std::string s = "test category ev";
//        const char* r = (const char*) s.c_str();
    return "marvin_category";
}
std::string boost::system::marvin_category::message(int ev) const
{
//    return "error message ev: " + std::to_string(ev);
        boost::system::marvin_error::marvin_error_code  e = static_cast<boost::system::marvin_error::marvin_error_code>(ev);
        switch( e ){
            case boost::system::marvin_error::marvin_error_code::end_of_message :
                return "end of message";
                break;
            case boost::system::marvin_error::marvin_error_code::end_of_body :
                return "end of body";
                break;
            case boost::system::marvin_error::marvin_error_code::ok:
                return "success";
                break;
            case boost::system::marvin_error::marvin_error_code::error_1:
                return "error_1";
                break;
            case boost::system::marvin_error::marvin_error_code::error_2:
                return "error_2";
                break;
            default:
                return "unknow Marvin error";
                break;
        }
        return "error message ";

}

static const class boost::system::marvin_category marvin_cat_const;

namespace boost {
namespace system{
    const error_category & marvin_category(){
        return marvin_cat_const;
    }
    namespace marvin_error
    {
        error_code make_error_code( marvin_error_code e ) {
            return error_code( e, marvin_category() );
        }
    }
    std::string error_description(boost::system::error_code& err)
    {
        std::stringstream ss;
        ss << "Error v: " << err.value()
                << " c: " << err.category().name()
                << " m: " << err.category().message(err.value());
        return ss.str();
    }

} // namespace system
} // namespace boost


namespace Marvin{
namespace Error{
    boost::system::error_code make_error_code(boost::system::marvin_error::marvin_error_code e) {
        return boost::system::marvin_error::make_error_code(e);
    }
    std::string error_description(boost::system::error_code& err)
    {
        return boost::system::error_description(err);
    }

    boost::system::error_code make_ok()     {
        return boost::system::marvin_error::make_error_code(boost::system::marvin_error::marvin_error_code::ok);
    }
    boost::system::error_code make_eom()    {
        return boost::system::marvin_error::make_error_code(boost::system::marvin_error::marvin_error_code::end_of_message);
    }
    boost::system::error_code make_eof()    {
        return boost::asio::error::make_error_code(boost::asio::error::misc_errors::eof);
    }
    boost::system::error_code make_eob()    {
        return boost::system::marvin_error::make_error_code(boost::system::marvin_error::marvin_error_code::end_of_body);
    }
    boost::system::error_code make_eparse() {
        return boost::system::marvin_error::make_error_code(boost::system::marvin_error::marvin_error_code::parser_error);
    }
}
}
