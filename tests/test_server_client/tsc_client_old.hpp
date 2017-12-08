//
//  test_client.h
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/13/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef test_client_h
#define test_client_h

#include <stdio.h>
#include "tsc_testcase.hpp"

void runTestClient();
void test_one(EchoTestcase& testcase);
void test_multiple(std::vector<EchoTestcase> cases);

template<class TESTEXEC, class TESTCASE>
void test_one(TESTCASE testcase)
{
    boost::asio::io_service io_service;
    TESTEXEC tst(io_service, testcase);
    tst.exec();
    io_service.run();
}
template<class TESTEXEC, class TESTCASE>
void test_multiple(std::vector<TESTCASE> cases)
{
    boost::asio::io_service io_service;
    
    std::vector<std::shared_ptr<TESTEXEC>> saved;
    for(TESTCASE& c : cases) {
        std::shared_ptr<TESTEXEC> texec = std::shared_ptr<TESTEXEC>(new TESTEXEC(io_service, c));
        saved.push_back(texec);
        texec->exec();
    }
    io_service.run();
}

#endif /* test_client_h */
