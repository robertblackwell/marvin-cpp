
#ifndef marvin_test_error_hpp
#define marvin_test_error_hpp
#include <string>
#include <iostream>
#include <boost/system/error_code.hpp>
/*!
* \defgroup Errors
* \brief Marvin may need to provide errors that are specific to the application; with this in mind boost::system::error_code was expanded with
* a new error_category (boost::system::marvin_category) so that marvin specific errors fit neatly into the boost system, read the deails.
*
*
*/
namespace boost{
namespace system{

    /**
    * \ingroup Errors
    */
    class marvin_category : public boost::system::error_category
    {
        const char* name() const noexcept;
        std::string message(int ev) const;
    };

    /**
    * \ingroup Errors
    */
    const error_category & marvin_category();
    namespace marvin_error {
        /**
        * \ingroup Errors
        */
        enum marvin_error_code {
            success = 0,
            error_1 = 42,
            error_2 = 43,
            ok = 0,
            end_of_message = 21,
            end_of_body = 22,
            parser_error = 23
        };
    }
    template<> struct is_error_code_enum<marvin_error::marvin_error_code>
        { static const bool value = true; };

    namespace marvin_error
    {
        /// \ingroup Errors
        error_code make_error_code( marvin_error_code e );
    }
}
}
namespace boost{
namespace system{
    /// \ingroup Errors
    std::string error_description(boost::system::error_code& err);
}
}
namespace Marvin{
    /// \ingroup Errors
    /// \brief This type alias is intended to provide a Marvin centric abbreviation for the type name of error codes; it does not however
    /// change the fact that all error_codes as boost::system::error_code
    using ErrorCode = boost::system::error_code;
/// \ingroup Errors
namespace Error{
    /// \ingroup Errors
    boost::system::error_code make_error_code(boost::system::marvin_error::marvin_error_code e);
    /// \ingroup Errors
    using marvin_error = boost::system::marvin_error::marvin_error_code;
    /// \ingroup Errors
    std::string error_description(boost::system::error_code& err);
    /// \ingroup Errors
    boost::system::error_code make_ok();
    /// \ingroup Errors
    boost::system::error_code make_eof();
    /// \ingroup Errors
    boost::system::error_code make_eom();
    /// \ingroup Errors
    boost::system::error_code make_eob();
    /// \ingroup Errors
    boost::system::error_code make_eparse();

}
}

/// headers end ===================================

#endif  // BOOST_SYSTEM_WINDOWS_ERROR_HPP

