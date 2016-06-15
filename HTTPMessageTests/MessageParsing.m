//
//  MessageParsing.m
//  Broker
//
//  Created by Robert Blackwell on 2/20/14.
//  Copyright (c) 2014 Blackwell Enterprises. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "BAHttpMessage.h"
#import "ChunkedParser.h"

@interface MessageParsing : XCTestCase

@end

@implementation MessageParsing

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

/*
 * Simple test - one non empty chunk and terminator all in one bite
 */
-(void) test1HeaderValues
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSString* teststr = @"GET /apath Http/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nProxy-Connection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    NSData * d = [teststr dataUsingEncoding:NSUTF8StringEncoding];

    BAHttpMessage* msg2 = [[BAHttpMessage alloc]init];
    msg2->cfMessage  = CFHTTPMessageCreateEmpty(NULL, false);
    //msg2->cfMessage = CFHTTPMessageCreateResponse(null, "201", "all is well", kCFHTTPVersion1_1);
    NSString* teststr2 = @"HTTP/1.1 200 OK\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    NSData * d2 = [teststr2 dataUsingEncoding:NSUTF8StringEncoding];
    [msg2 appendData:d2];
    NSString* vcv2 = [msg2 statusLine];
    NSURL* uuu = [msg2 requestURL];
    NSString* mm = [msg2 method];

    BAHttpMessage* msg3 = [[BAHttpMessage alloc]init];
    msg3->cfMessage  = CFHTTPMessageCreateEmpty(NULL, true);
    //msg2->cfMessage = CFHTTPMessageCreateResponse(null, "201", "all is well", kCFHTTPVersion1_1);
    NSString* teststr3 = @"GET /apath HTTP/1.1\r\nHost: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n";
    teststr3 = @"GET /apath";
    //teststr3 = @"HTTP/1.1 200";
    NSData * d3 = [teststr3 dataUsingEncoding:NSUTF8StringEncoding];
    [msg3 appendData:d3];
    NSString* vcv3 = [msg3 statusLine];
    NSInteger dsd3 = CFHTTPMessageGetResponseStatusCode(msg3->cfMessage);
    NSURL* uuu3 = [msg3 requestURL];
    NSString* mm3 = [msg3 method];
    
    
    [msg appendData:d];
    XCTAssert([msg contentLength] == 10, @"contentLength");
    id xxx = [msg requestURL];
    id yxy = [xxx absoluteString];
    id yuy = [xxx path];
    NSString* vcv = [msg statusLine];
    
    XCTAssert([[[msg requestURL] absoluteString] isEqualToString:@"http://ahost/apath"], @" path");
    XCTAssert([[msg host] isEqualToString:@"ahost"], @"host");
    BOOL tt = [[msg connection] isEqualToString:@"keep-alive"];
    XCTAssertTrue([[msg connection] isEqualToString:@"keep-alive"], @"connection");
    XCTAssertTrue([[msg proxyConnection] isEqualToString:@"keep-alive"], @"Proxy connection");
    BOOL t = [[msg bodyBuffer] isEqualToData: [@"" dataUsingEncoding:NSUTF8StringEncoding]];
    XCTAssertTrue(t,@"");
    
}
-(void) test1Parse
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    
    NSString* teststr = @"GET / Http/1.1\r\nContent-Length: 0\r\n\r\n";
    
    NSData * d = [teststr dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:d];
    XCTAssert([msg messageIsComplete], @"message should be complete");
    XCTAssert([msg headerIsComplete], @"header should be complete");
    XCTAssert([msg contentLengthHeaderExists], @"coontentLengthHeader xists");
    XCTAssertFalse([msg chunksAreComplete], @"this is not a chunked message");
    XCTAssertFalse([msg messageIsChunked], @"message is not chunked");
    BOOL t = [[msg bodyBuffer] isEqualToData: [@"" dataUsingEncoding:NSUTF8StringEncoding]];
    XCTAssertTrue(t,@"");

}
-(void) testRequestresponseDetection_01
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSString* teststr = @" GET / Http/1.1\r\nContent-Length: 10\r\n\r\n0123456789";
    NSData * d = [teststr dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:d];
    
}
-(void) testRequestresponseDetection_02
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSString* teststr = @" HTTP/1.1 200 OK\r\nContent-Length: 10\r\n\r\n0123456789";
    NSData * d = [teststr dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:d];
    
}
-(void) test1_01Parse
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    
    NSString* teststr = @"GET / Http/1.1\r\nContent-Length: 10\r\n\r\n0123456789";
    NSData * d = [teststr dataUsingEncoding:NSUTF8StringEncoding];
    [msg appendData:d];
    XCTAssert([msg messageIsComplete], @"message should be complete");
    XCTAssert([msg headerIsComplete], @"header should be complete");
    XCTAssert([msg contentLengthHeaderExists], @"coontentLengthHeader xists");
    XCTAssert([msg contentLength] == 10, @"contentLength");
    XCTAssertFalse([msg chunksAreComplete], @"this is not a chunked message");
    XCTAssertFalse([msg messageIsChunked], @"message is not chunked");
    BOOL t = [[msg bodyBuffer] isEqualToData: [@"0123456789" dataUsingEncoding:NSUTF8StringEncoding]];
    XCTAssertTrue(t,@"");
}
-(void) test1_02Parse
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* input = @[
                       @"HTTP/1.1 200 OK\r\n",
                       @"Transfer-Enco",
                       @"ding: chunked\r\n\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890XXXXX\r\n",
                       @"0a\r\n1234567890YYYYY\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0a\r\n1234567890HGHGH\r\n",
                       @"0a\r\n1234567890\r\n",
                       @"0\r\n",
                       @"\r\n"];
    for(NSString* s in input){
        NSData * d = [s dataUsingEncoding:NSUTF8StringEncoding];
        [msg appendData:d];
    }
    XCTAssertTrue([msg chunksAreComplete], @"this is not a chunked message");
    XCTAssertTrue([msg messageIsChunked], @"message is not chunked");
    XCTAssertTrue([msg messageIsComplete], @"message should be complete");
    XCTAssertTrue([msg headerIsComplete], @"header should be complete");
    printf("");
}


@end
