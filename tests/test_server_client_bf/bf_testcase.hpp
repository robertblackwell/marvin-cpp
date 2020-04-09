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
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/trog/trog.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/client/client.hpp>

namespace body_format {
    enum class TestType {
        WITH_STRING,
        WITH_MBUFFER,
        WITH_BUFFER_CHAIN
    };
    class Testcase
    {
        public:
        TestType                    m_test_type;
        std::vector<std::string>    m_body_buffers;
        std::string                 m_url;
        std::string                 m_description;
        
        Testcase(TestType type, std::string desc, std::vector<std::string>);
        std::size_t     chunks_count() const;
        Marvin::MBufferSPtr     buffers_as_mbuffer() const;
        std::string     buffers_as_string() const;
        Marvin::BufferChainSPtr buffers_as_buffer_chain() const;
        
        Marvin::MBufferSPtr     chunk_as_mbuffer(std::size_t index);
        std::string     chunk_as_string(std::size_t index);
        Marvin::BufferChainSPtr chunk_as_buffer_chain(std::size_t index);

    };
}
#endif
