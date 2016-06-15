//
//
//
//
//
#import <XCTest/XCTest.h>
#import "HTTPMessage.h"
#import "HTTPParser.h"

@interface TestParsing : XCTestCase

@property HTTPMessage* parsedMessage;
@property NSDictionary* parsedHeaders;
@property bool         errorWhileParsing;
@property NSInteger     numberOfParsedMessages;

@end

@implementation TestParsing

@synthesize  parsedMessage;
@synthesize  parsedHeaders;
@synthesize  errorWhileParsing;
@synthesize numberOfParsedMessages;

- (void)setUp
{
    parsedMessage = nil;
    errorWhileParsing = false;
    parsedHeaders = nil;
    numberOfParsedMessages = 0;
    
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
-(void) parserDidFinishParsingHeaders:(HTTPMessage*)message
{
    parsedHeaders = [NSDictionary dictionaryWithDictionary:message.headers];
    NSLog(@"");
}
-(void) parserDidFinishParsingMessage:(HTTPMessage*)message
{
    parsedMessage = message;
    numberOfParsedMessages++;
    NSLog(@"");
}
-(void) parserError:(NSError*) error
{
    errorWhileParsing = true;
    NSLog(@"");
}

-(void) loadTestStrings:(NSArray*) testStrings
{
    HTTPParser* parser = [[HTTPParser alloc]init];
    [parser setDelegate:self];
    for(NSString* testString in testStrings)
    {
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        NSInteger countParsed = [parser appendData:testData];
        if( countParsed != [testData length])
        {
            errorWhileParsing = true;
            break;
        }
        NSLog(@"");
    }
    
}
#ifdef JJJJJ
-(void) xtestParseSimpleGETMessage
{
    NSArray* testStrings = @[
                             @"GET /apath HTTP/1.1\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             ];
    
    
    HTTPParser* parser = [[HTTPParser alloc]init];
    [parser setDelegate:self];
    for(NSString* testString in testStrings)
    {
        NSLog(@"testString: %@", testString);
        NSData * testData = [testString dataUsingEncoding:NSUTF8StringEncoding];
        NSInteger countParsed = [parser appendData:testData];
        NSLog(@"");
    }
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    XCTAssert([parsedMessage.headers count] == 2);
    
    XCTAssertTrue( kHttpMethodGET == parsedMessage.method,@"http method is bad " );
    XCTAssertTrue( parsedMessage.http_major == 1 ,@" http major version is bad" );
    XCTAssertTrue( parsedMessage.http_minor == 1 ,@" http minor version is bad" );
    XCTAssertTrue( [@"/apath" isEqualToString:parsedMessage.url],@" url path is bad" );
    NSLog(@"");
}
- (void)testSimpleResponseWithBody1
{
    NSArray* testStrings = @[
//                             @"GET /apath HTTP/1.1\r\n",
//                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"\r\n",
                             @"0123456789"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");

    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( [parsedMessage.body length] == 10, @" bad message body length");
}
- (void)testSimpleResponseWithBody2
{
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 10\r\n\r\n0123456789"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 10, @" bad message body length");
}
- (void)testSimpleResponseWithOutBody1
{
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 0\r\n",
                             @"\r\n"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 0, @" bad message body length");
}

- (void)testResponseTransferEncoding
{
    //
    // There is a problem with this result :
    //
    //  Header
    //      -   still contains Transfer Encoding: chunked
    //      -   does not contain a Content Length header
    //      -   the body is no longer in "chunked" format
    //
    // Means the message is NOT suitable for retransmission yet.
    //
    
    NSArray* testStrings = @[
                             @"HTTP/1.1 201 OK Reason Phrase \r\n",
                             @"Host: ahost\r\n",
                             @"Connection: keep-alive\r\n",
                             @"Proxy-Connection: keep-alive\r\n",
                             @"Transfer-Encoding: chunked\r\n",
                             @"\r\n",
                             @"0a\r\n1234567890\r\n",
                             @"0b\r\n2234567890a\r\n",
                             @"0c\r\n3234567890ab\r\n",
                             @"0d\r\n4234567890abc\r\n",
                             @"09\r\n523456789\r\n",
                             @"08\r\n62345678\r\n",
                             @"07\r\n7234567\r\n",
                             @"0a\r\n8234567890\r\n",
                             @"0a\r\n9234567890\r\n",
                             @"0a\r\na234567890\r\n",
                             @"0a\r\nb234567890\r\n",
                             @"0a\r\nc234567890\r\n",
                             @"0\r\n",
                             @"\r\n"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 120, @" bad message body length");
    
    NSString* tfec = [parsedMessage.headers objectForKey:@"Transfer-Encoding"];
    
    XCTAssert( [parsedMessage.headers objectForKey:@"Transfer-Encoding"] != nil, @"TRansfer-Encoding header does not exists");
    }

- (void)testRequestWithCookie1
{
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Cookie: key=value;path=/;fred=jim\r\n",
                             @"Content-Length: 0\r\n",
                             @"\r\n"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 0, @" bad message body length");
    
    NSString* cooki = [parsedMessage.headers objectForKey:@"Cookie"];
    
    XCTAssert( [parsedMessage.headers objectForKey:@"Cookie"] != nil, @"should have a cookie header");

}
- (void)testRequestWithLongishCookie
{
    
    NSString* cookieString = @"key=0000";
    for(int i = 0; i < 100; i++){
        cookieString = [NSString stringWithFormat:@"%@;key%03d=%03d", cookieString, i, i];
    }
    int ll = [cookieString length];
    
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Cookie: ",
                             cookieString,
                             @"\r\n",
                             @"Content-Length: 0\r\n",
                             @"\r\n"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 0, @" bad message body length");
    
    NSString* cooki = [parsedMessage.headers objectForKey:@"Cookie"];
    
    XCTAssert( [parsedMessage.headers objectForKey:@"Cookie"] != nil, @"should have a cookie header");
    
}


- (void)testRequestWithVeryLongCookie // Trigger SimpleBuffer into re-allocating buffer space
{
    
    NSString* cookieString = @"key=0000";
    for(int i = 0; i < 250; i++){
        cookieString = [NSString stringWithFormat:@"%@;key%03d=%03d", cookieString, i, i];
    }
    int ll = [cookieString length];
    
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Cookie: ",
                             cookieString,
                             @"\r\n",
                             @"Content-Length: 0\r\n",
                             @"\r\n"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    XCTAssert( parsedMessage.status_code == 200, @" bad status " );
    XCTAssert( parsedMessage.body != nil, @" bad message body cannot be nil");
    XCTAssert( [parsedMessage.body length] == 0, @" bad message body length");
    
    NSString* cooki = [parsedMessage.headers objectForKey:@"Cookie"];
    
    XCTAssert( [parsedMessage.headers objectForKey:@"Cookie"] != nil, @"should have a cookie header");
    
}

- (void)testResponseWithExtraData1
{
    NSArray* testStrings = @[
                             //                             @"GET /apath HTTP/1.1\r\n",
                             //                             @"Host: ahost\r\nConnection: keep-alive\r\n\r\n"
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 11\r\n",
                             @"\r\n",
                             @"01234567891"
                             @"0123456789"
                             ];
    [self loadTestStrings:testStrings];
    //
    // We got the message
    //
    XCTAssert(parsedMessage != nil, @" message not parsed");
    
    //
    // But the extra data - in a format that is NOT a valid message - caused an error
    //
    XCTAssert(errorWhileParsing == true, @" unexpected error while parsing");
    
}
#endif 

- (void)testResponseConsecutiveMessages1
{
    NSArray* testStrings = @[
                             @"HTTP/1.1 200 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 10\r\n",
                             @"\r\n",
                             @"0123456789"
                             @"HTTP/1.1 201 OK\r\n",
                             @"Host: ahost\r\nConnection: keep-alive\r\n",
                             @"Content-Length: 15\r\n",
                             @"\r\n",
                             @"012345678912345"
                             ];
    [self loadTestStrings:testStrings];
    
    XCTAssert(parsedMessage != nil, @" message not parsed");
    XCTAssert(errorWhileParsing == false, @" unexpected error while parsing");
    
    //
    // We remembered the second messages because it overwrote the first.
    //
    XCTAssert( self.numberOfParsedMessages == 2, @" wrong number of parsed messages " );
    XCTAssert( parsedMessage.status_code == 201, @" bad status " );
    XCTAssert( [parsedMessage.body length] == 15, @" bad message body length");
}





@end
