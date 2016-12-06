#ifndef mock_error_hpp
#define mock_error_hpp

#include <iostream>
#include <iterator>
#include <algorithm>
#include "catch.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "marvin_error.hpp"


class Error
{
public:
    Error(std::string msg): value(msg)
    {
        value = msg;
    }
    Error& operator=(Error other)
    {
        return *this;
    }
    static Error* success(){
        Error* e = new Error("OK");
        return e;
    }
    static Error eom(){
        Error e{"EOM"};
        return e;
    }
    static Error* end_of_message(){
        Error* e = new Error("EOM");
        return e;
    }
    static Error ok(){
        Error e{"OK"};
        return e;
    }
    bool equalTo(Error& rhs){ return this->value == rhs.value; }
    
    bool operator ==(Error& rhs) const{
        return (this->value == rhs.value);
    }
private:
    std::string value;
};

#endif