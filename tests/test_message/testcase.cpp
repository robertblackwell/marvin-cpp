#include "testcase.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <map>




Testcase::Testcase(
    std::string                     description,
    std::vector<std::string>        rawMessage,
    std::string                     result_first_line,
    int                             status_code,
    Marvin::ErrorType               err,
    Marvin::HeadersV2::Initializer  result_headers,
    std::string                     result_body
)
    :
    m_rawMessage(rawMessage),
    m_result_first_line(result_first_line),
    m_result_headers_vec(result_headers),
    m_result_body(result_body)
    {
        m_result_headers = m_result_headers_vec;
        m_description = description;
        m_index = 0;
        m_result_status_code = status_code;
        m_result_onheaders_err = err;
    }
    std::string Testcase::getDescription()
    {
        return m_description;
    }
    std::string Testcase::lineAt(std::size_t ix)
    {
        assert(ix < m_rawMessage.size());
        std::string line = m_rawMessage[ix];
        return line;
    }
    std::vector<std::string> Testcase::buffers()
    {
        return m_rawMessage;
    }
    bool Testcase::verify_first_line(std::string fl)
    {
        return (fl == m_result_first_line);
    }
    bool Testcase::verify_headers(::Marvin::HeadersV2& h)
    {
        return h.sameValues(m_result_headers);
    }
    bool Testcase::verify_body(std::string b)
    {
        return (b == m_result_body);
    }
    std::string Testcase::result_first_line()
    {
        return m_result_first_line;
    }
    int Testcase::result_status_code()
    {
        return m_result_status_code;
    }
    Marvin::ErrorType Testcase::result_onheaders_err()
    { 
        return m_result_onheaders_err; 
    }
    Marvin::HeadersV2& Testcase::result_headers()
    {
        return m_result_headers;
    }
    std::string Testcase::result_body()
    {
        return m_result_body;
    }
    std::string Testcase::next()
    {
        std::string line = lineAt(m_index);
        m_index++;
        return line;
    }
    bool Testcase::finished()
    {
        bool r = (m_index >= m_rawMessage.size() );
        return r;
    }
    bool Testcase::is_error_case(){return false;}

    std::string case_result(){ return "";}
