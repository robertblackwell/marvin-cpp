#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "testcase.hpp"

Testcase::Testcase(
    std::string description,
    std::vector<std::string> rawMessage,
    std::string result_first_line,
    int status_code,
    Marvin::ErrorType err,
    Marvin::Http::Headers::Initializer result_headers,
    std::string result_body
)
    :
    _rawMessage(rawMessage),
    _result_first_line(result_first_line),
    _result_headers_vec(result_headers),
    _result_body(result_body)
    {
        _result_headers = _result_headers_vec;
        _description = description;
        _index = 0;
        _result_status_code = status_code;
        _result_onheaders_err = err;
    }
    std::string Testcase::getDescription()
    {
        return _description;
    }
    std::string Testcase::lineAt(std::size_t ix)
    {
        assert(ix < _rawMessage.size());
        std::string line = _rawMessage[ix];
        return line;
    }
    std::vector<std::string> Testcase::buffers()
    {
        return _rawMessage;
    }
    bool Testcase::verify_first_line(std::string fl)
    {
        return (fl == _result_first_line);
    }
    bool Testcase::verify_headers(::Marvin::Http::Headers& h)
    {
        return (_result_headers == h);
    }
    bool Testcase::verify_body(std::string b)
    {
        return (b == _result_body);
    }
    std::string Testcase::result_first_line(){
        return _result_first_line;
    }
    int Testcase::result_status_code(){return _result_status_code;}

    Marvin::ErrorType Testcase::result_onheaders_err(){ return _result_onheaders_err; }

    Marvin::Http::Headers& Testcase::result_headers(){
        return _result_headers;
    }
    std::string Testcase::result_body(){
        return _result_body;
    }
    std::string Testcase::next()
    {
        std::string line = lineAt(_index);
        _index++;
        return line;
    }
    bool Testcase::finished()
    {
        bool r = (_index >= _rawMessage.size() );
        return r;
    }
    bool Testcase::is_error_case(){return false;}

    std::string case_result(){ return "";}
