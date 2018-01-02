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
#include "boost_stuff.hpp"
#include "rb_logger.hpp"
#include "buffer.hpp"
#include "client.hpp"

namespace tp {
    class Testcase;
    using TestcaseSPtr = std::shared_ptr<tp::Testcase>;
    class Testcase
    {
        public:
        std::string                 m_proxy_scheme;
        std::string                 m_proxy_host;
        std::string                 m_proxy_port;
        MessageBaseSPtr             m_msg_sptr;
        
        Testcase(MessageBaseSPtr msgSPtr, std::string scheme, std::string proxyHost, std::string proxyPort);
    };
}
#endif
