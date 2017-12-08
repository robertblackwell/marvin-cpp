//
//  test_client.c
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/13/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//
#include "tsc_client.hpp"
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

#include "bufferV2.hpp"
#include "client.hpp"
#include "tsc_testcase.hpp"
#include "tsc_post.hpp"
#include "tsc_get.hpp"

EchoTestcase  tcase1(
    EchoTestType::WITH_STRING,
    "With string - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);
EchoTestcase  tcase2(
    EchoTestType::WITH_MBUFFER,
    "With mbuffer - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);
EchoTestcase  tcase3(
    EchoTestType::WITH_BUFFER_CHAIN,
    "With buffer chain - 2 buffers",
    {
        "1234567890",
        "HGYTRESAWQ"
    }
);

