//
//  HeaderFieldsAccess.m
//  Broker
//
//  Created by Robert Blackwell on 2/20/14.
//  Copyright (c) 2014 Blackwell Enterprises. All rights reserved.
//
/*
 * This file tests that the properties of BAHttpMessage that read header field values
 * work correctly.
 *
 * In these tests no importance is placed on the nature of the chunks of data that are passed to
 * the messages appendData method.
 *
 */
#import <XCTest/XCTest.h>
#import "HTTPMessage.h"
#import "HTTPParser.h"

@interface HeaderFieldsAccess : XCTestCase

@end

@implementation HeaderFieldsAccess

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
-(void) dumpHeaderValues:(NSDictionary*) dict
{
    for(id key in dict){
        NSLog(@"key: %@ value: %@ ", key, [dict objectForKey:key]);
    }
}
-(void) testMessageParsing
{
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n"
                             ];

    
    for(NSString* testString in testStrings)
    {
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        HTTPParser* parser = [[HTTPParser alloc]init];
        NSInteger countParsed = [parser appendData:testData];
    }
}
#ifdef TTTTT
- (void)testRequestFirstLine
{
    HTTPMessage* msg = [[HTTPMessage alloc]init];
    NSArray* testStrings = @[
                             @"GET /apath HTTP/1.1\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];

    XCTAssertTrue( [kHttp_GET_Method isEqualToString:[msg method]],@"" );
    XCTAssertTrue( [kHttp_Version_1_1 isEqualToString:[msg version]],@"" );
    XCTAssertTrue( [@"http://ahost/apath" isEqualToString:[[msg requestURL] absoluteString]],@"" );
}
- (void)testResponseFirstLine
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    
    XCTAssertTrue( [kHttp_Version_1_1 isEqualToString:[msg version]],@"%@", [msg version] );
    XCTAssertTrue( [@"201" isEqualToString:[msg statusCode]],@"" );
}
- (void)testResponseMod_Remove_ProxyConnection
{
    HTTPMessage* msg = [[HTTPMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    NSDictionary* before = [msg allHeaderValues];
    [self dumpHeaderValues: before ];
    NSString* p_before = [msg proxyConnection];
    [msg setProxyConnection:nil];
    NSString* p_after = [msg proxyConnection];
    
    XCTAssertTrue([@"keep-alive" isEqualToString:p_before],@"");
    XCTAssertTrue(p_after == nil, @"");
    
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
    
}
- (void)testResponseMod_SetConnectionToClose
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    NSDictionary* before = [msg allHeaderValues];
    [self dumpHeaderValues: before ];
    NSString* p_before = [msg connection];
    [msg setConnection:@"close"];
    NSString* p_after = [msg connection];
    
    XCTAssertTrue([@"keep-alive" isEqualToString:p_before],@"");
    XCTAssertTrue([@"close" isEqualToString:p_after],@"");
    
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
    
}
- (void)testResponseMod_TransferEncoding
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n"];
    NSArray* bodyStrings = @[
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
                             @"0\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    NSDictionary* before = [msg allHeaderValues];
    [self dumpHeaderValues: before ];
    NSString* p_before = [msg transferEncoding];
    [msg setTransferEncoding:nil];
    NSString* p_after = [msg transferEncoding];
    
    XCTAssertTrue([@"chunked" isEqualToString:p_before],@"");
    XCTAssertTrue( p_after == nil ,@"");
    
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
    
}
- (void)testResponseMod_BrokerRemovePorxyConnectionMakeConnectionClose
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    NSDictionary* before = [msg allHeaderValues];
    [self dumpHeaderValues: before ];
    NSString* p_before = [msg proxyConnection];
    [msg setProxyConnection:nil];
    NSString* p_after = [msg proxyConnection];
    NSString* c_before = [msg connection];
    [msg setConnection:@"close"];
    NSString* c_after = [msg connection];
    NSString* k_before = [msg transferEncoding];
    [msg setTransferEncoding:nil];
    NSString* k_after = [msg transferEncoding];
    
    XCTAssertTrue([@"keep-alive" isEqualToString:c_before],@"");
    XCTAssertTrue([@"close" isEqualToString:c_after],@"");
    XCTAssertTrue([@"keep-alive" isEqualToString:p_before],@"");
    XCTAssertTrue(p_after == nil, @"");
    XCTAssertTrue([@"chunked" isEqualToString:k_before],@"");
    XCTAssertTrue( k_after == nil ,@"");
    
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
    
}
-(NSString*) concatStrings:(NSArray*)strings
{
    NSMutableString* r = [[NSMutableString alloc]init];
    for(NSString* s in strings){
        [r appendString:s];
    }
    return r;
}
- (void)testResponsSerialize_dechunk
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n",
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
                             @"0\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    //
    // Now convert to a non-chunked message
    //
    NSDictionary* before = [msg allHeaderValues];
    [self dumpHeaderValues: before ];
    NSString* p_before = [msg transferEncoding];
    
    [msg setTransferEncoding:nil];
    [msg setProxyConnection:nil];
    [msg setConnection:nil];
    
    NSString* p_after = [msg transferEncoding];
    
    XCTAssertTrue([@"chunked" isEqualToString:p_before],@"");
    XCTAssertTrue( p_after == nil ,@"");
    NSData* dech = msg->chunkedParser.deChunkedBuffer;
    NSInteger bl = [dech length];
    [msg setContentLength:bl];
    NSData* ser = [msg serializeDeChunk:true];
    NSString* serStr = [[NSString alloc]initWithData:ser encoding:NSUTF8StringEncoding];
    
    NSArray* ts = @[
    @"HTTP/1.1 201 OK Reason Phrase\r\n",
    @"Host: ahost\r\n",
    @"Content-Length: 120\r\n"
    @"\r\n",
    @"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
    ];
    NSString* cs = [self concatStrings:ts];
    BOOL xx = [serStr isEqualToString:cs];
    XCTAssertTrue(xx, @"");
    //NSLog(@"\n%@",serStr);
    //NSLog(@"\n%@",cs);
    //[self compareStrings:serStr b:cs];
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
}

- (void)testResponsSerialize_No_dechunk
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase\r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n",
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
                             @"\r\n"
                             ];
    NSArray* ts = @[
                             @"HTTP/1.1 201 OK Reason Phrase\r\n",
                             @"Host: ahost\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"Connection: keep-alive\r\n",
                             @"\r\n",
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
                             @"0\r\n"
                             @"\r\n"
                             ];
    [self loadMessage:msg strings:testStrings];
    
    [msg setProxyConnection:nil];
    
    NSData* ser = [msg serializeDeChunk:false];
    NSString* serStr = [[NSString alloc]initWithData:ser encoding:NSUTF8StringEncoding];
    
    NSString* cs = [self concatStrings:ts];
    BOOL xx = [serStr isEqualToString:cs];
    XCTAssertTrue(xx, @"");
    NSLog(@"\n%@",serStr);
    NSLog(@"\n%@",cs);
    [self compareStrings:serStr b:cs];
    NSDictionary* after = [msg allHeaderValues];
    [self dumpHeaderValues: after ];
}

- (void)testResponsSerialize_MessageNotChunk
{
    BAHttpMessage* msg = [[BAHttpMessage alloc]init];
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase\r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Content-length: 50\r\n",
                             @"\r\n",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890"
                             ];
    NSArray* ts = @[
                             @"HTTP/1.1 201 OK Reason Phrase\r\n",
                             @"Host: ahost\r\n",
                             @"Connection: close\r\n",
                             @"Content-Length: 50\r\n",
                             @"\r\n",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890",
                             @"1234567890"
                             ];
    [self loadMessage:msg strings:testStrings];
    
    [msg setTransferEncoding:nil];
    [msg setProxyConnection:nil];
    [msg setConnection:@"close"];
    
    
    NSData* ser = [msg serializeDeChunk:false];
    NSString* serStr = [[NSString alloc]initWithData:ser encoding:NSUTF8StringEncoding];
    
    NSString* cs = [self concatStrings:ts];
    BOOL xx = [serStr isEqualToString:cs];
    XCTAssertTrue(xx, @"");
    //NSLog(@"\n%@",serStr);
    //NSLog(@"\n%@",cs);
//    [self compareStrings:serStr b:cs];
//    NSDictionary* after = [msg allHeaderValues];
//    [self dumpHeaderValues: after ];
}

-(BOOL) compareStrings:(NSString*)a b:(NSString*)b
{
//    if([a length] != [b length]){
//        NSLog(@" not the same length a: %ld b: %ld", [a length], [b length]);
//        return false;
//    }
    NSInteger i;
    for(i = 0; i < [a length]; i++){
        if( [a characterAtIndex:i] != [b characterAtIndex:i]){
            
            unsigned short a_c = [a characterAtIndex:i];
            unsigned short b_c = [b characterAtIndex:i];
            
            NSLog(@"[%c(%d) != %c(%d)", [a characterAtIndex:i],[a characterAtIndex:i], [b characterAtIndex:i], [b characterAtIndex:i]);
            return false;
        }else{
            NSLog(@"%c(%d)", [b characterAtIndex:i],[b characterAtIndex:i]);
        }
    }
    return true;
}
#endif
@end
