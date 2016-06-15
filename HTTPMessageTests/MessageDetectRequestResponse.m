//
//  MessageDetectRequestResponse.m
//  Broker
//
//  Created by Robert Blackwell on 2/20/14.
//  Copyright (c) 2014 Blackwell Enterprises. All rights reserved.
//
/*
 * These tests are to verify that the BAHttpMessage class correctly detects the type of message request/response
 * during the early stages of parsing and hence creates the correct flavour of CFHTTPMessage.
 *
 * In particular the tests examine the processing when the initial append operation does not provide enough data
 * to be definitiev and the message must save context and wait for more data before deciding.
 *
 */
#import <XCTest/XCTest.h>
#import "BAHttpMessage.h"

@interface MessageDetectRequestResponse : XCTestCase

@end

@implementation MessageDetectRequestResponse

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testRequest_01
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSString* testString = @"GET /apath Http/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:testData];
    XCTAssertTrue(msg.isRequest, @" detect a request");
    XCTAssertTrue( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}
- (void)testRequest_02
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                @"GET",
                @" /apath",
                @"Http/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                ];
    for(NSString* testString in testStrings){
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        [msg appendData:testData];
    }
    XCTAssertTrue( msg.isRequest, @" detect a request" );
    XCTAssertTrue( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}
- (void)testRequest_03
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"",
                             @"GET",
                             @" /apath",
                             @"Http/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                             ];
    for(NSString* testString in testStrings){
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        [msg appendData:testData];
    }
    XCTAssertTrue(msg.isRequest, @" detect a request");
    XCTAssertTrue( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}
- (void)testResponse
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSString* testString = @"HTTP/1.1 200 OK This is a message\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:testData];
    XCTAssertFalse(msg.isRequest, @" detect a request");
    XCTAssertFalse( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}
- (void)testResponse_02
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTT",
                             @"P/1.1 200",
                             @" OK \r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                             ];
    for(NSString* testString in testStrings){
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        [msg appendData:testData];
    }
    XCTAssertFalse( msg.isRequest, @" detect a request" );
    XCTAssertFalse( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}
- (void)testResponse_03
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"",
                             @"HTT",
                             @"P/1.1 200",
                             @" OK \r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                             ];
    for(NSString* testString in testStrings){
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        [msg appendData:testData];
    }
    XCTAssertFalse(msg.isRequest, @" detect a request");
    XCTAssertFalse( CFHTTPMessageIsRequest(msg->cfMessage), @"");
}

@end
