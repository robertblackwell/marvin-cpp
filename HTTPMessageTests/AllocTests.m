//
//  HTTPMessageTests.m
//  HTTPMessageTests
//
//  Created by ROBERT BLACKWELL on 10/15/15.
//  Copyright © 2015 Blackwellapps. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "HTTPMessage.h"
#import "HTTPParser.h"

@interface HTTPMessageTests : XCTestCase

@end

@implementation HTTPMessageTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testHTTPMessageAllocInit {
    HTTPMessage* newMessage = [[HTTPMessage alloc]init];
    XCTAssert(newMessage != nil);
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
