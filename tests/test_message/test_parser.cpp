
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/asio.hpp>

#include <doctest/doctest.h>
#include <marvin/http/message_base.hpp>
#include <marvin/http/parser.hpp>

#include "helper_types.hpp"
#include "data_two.hpp"

namespace Marvin {
namespace Tests {
namespace Message {

TEST_CASE("parser_on_test_data")
{
    TestCollection& tc = test_data_two_message();
    for(auto entry: tc) {
        std::string key = entry.first;
        TestSet ts = entry.second;
        std::cout << "Commencing test of Parser test data set: " << ts.description  << std::endl;
        DataSource data_source(ts.input_data);
        Marvin::Parser parser;
        WrappedParserTest wpt(parser, data_source, ts.verify_function);
        wpt();
        std::cout << "Completed test of Parser test data set: " << ts.description  << std::endl;
    }
}


} // namespace Parser
} // namespace Tests
} // namespace Marvin