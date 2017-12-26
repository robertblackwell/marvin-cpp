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
    /// \ingroup OriginalErrors
    typedef boost::system::error_code ErrorType;
    
    /// \ingroup OriginalErrors
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
    
    /// \ingroup OriginalErrors
    const boost::system::error_category& category();
    /// \ingroup OriginalErrors
    boost::system::error_code make_error_code(Marvin::errc e);
    /// \ingroup OriginalErrors
    boost::system::error_condition make_error_condition(Marvin::errc e);
    
} // namespace Marvin

namespace boost {
namespace system {
        template <>
        struct is_error_code_enum<Marvin::errc>
        : public std::true_type {};
}} // namespace boost::system

namespace Marvin{
    
    /// \ingroup OriginalErrors
    ErrorType make_error_ok();
    /// \ingroup OriginalErrors
    ErrorType make_error_eof();
    /// \ingroup OriginalErrors
    ErrorType make_error_eom();
    /// \ingroup OriginalErrors
    ErrorType make_error_eob();
    /// \ingroup OriginalErrors
    ErrorType make_error_parse();
    /// \ingroup OriginalErrors
    std::string make_error_description(Marvin::ErrorType& err);
} // namespace Marvin

namespace Marvin{
namespace Error{
    /// \ingroup OriginalErrors
    ErrorType  make_ok();
    /// \ingroup OriginalErrors
        ErrorType  make_eof();
    /// \ingroup OriginalErrors
        ErrorType  make_eom();
    /// \ingroup OriginalErrors
        ErrorType  make_eob();
    /// \ingroup OriginalErrors
        ErrorType  make_eparse();
}} // namespace Marvin::Error


#endif
