#ifndef tsc_testcase_hpp
#define tsc_testcase_hpp
//
//  main.cpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/18/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "rb_logger.hpp"
#include "buffer.hpp"
#include "client.hpp"

namespace body_format {
    enum class TestType {
        WITH_STRING,
        WITH_MBUFFER,
        WITH_BUFFER_CHAIN
    };
    class Testcase
    {
        public:
        TestType                    _test_type;
        std::vector<std::string>    _body_buffers;
        std::string                 _url;
        std::string                 _description;
        
        Testcase(TestType type, std::string desc, std::vector<std::string>);
        std::size_t     chunks_count() const;
        MBufferSPtr     buffers_as_mbuffer() const;
        std::string     buffers_as_string() const;
        BufferChainSPtr buffers_as_buffer_chain() const;
        
        MBufferSPtr     chunk_as_mbuffer(std::size_t index);
        std::string     chunk_as_string(std::size_t index);
        BufferChainSPtr chunk_as_buffer_chain(std::size_t index);

    };
}
#endif
