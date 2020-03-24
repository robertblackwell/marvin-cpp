#ifndef MARVIN_ERROR_HPP
#define MARVIN_ERROR_HPP
/** \defgroup OriginalErrors
 * \brief This file defines a few Marvin specific error codes based on boost::system::error_code
 *
 *  To save typing I have typdef'd boost::system::error_code as Marvin::ErrorType
 *  and provided utility functions for creating a small set of standard and custom
 *  error codes
*/
#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
namespace Marvin{
    typedef boost::system::error_code ErrorType;
    
    enum class errc {
        ok = 0,
        end_of_message = 21,
        end_of_body = 22,
        parser_error = 23
    };

    /// \ingroup OriginalErrors
    class category : public boost::system::error_category
    {
    public:
        const char *name() const noexcept;
        std::string message(int ev) const;
    };


//    static Marvin::category cat;
    
    const boost::system::error_category& category();
    boost::system::error_code make_error_code(Marvin::errc e);
    boost::system::error_condition make_error_condition(Marvin::errc e);
    
} // namespace Marvin

namespace boost {
namespace system {
        template <>
        struct is_error_code_enum<Marvin::errc>
        : public std::true_type {};
}} // namespace boost::system

namespace Marvin{
    
    ErrorType make_error_ok();
    ErrorType make_error_eof();
    ErrorType make_error_eom();
    ErrorType make_error_eob();
    ErrorType make_error_parse();
    std::string make_error_description(const Marvin::ErrorType& err);
} // namespace Marvin

namespace Marvin{
namespace Error{
    ErrorType  make_ok();
    ErrorType  make_eof();
    ErrorType  make_eom();
    ErrorType  make_eob();
    ErrorType  make_eparse();
}} // namespace Marvin::Error

#endif
