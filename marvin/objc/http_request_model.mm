//
//  RequestModel.m
//  Broker
//
//  Created by Brendon Blackwell on 18/01/14.
//  Copyright (c) 2014 Robert Blackwell. All rights reserved.
//

#import "http_request_model.h"

@implementation HttpRequestModel

- (id) init
{
    if ( self = [super init] ) {
            self.minorVersion = nil;
            self.method = nil;
            self.uri = nil;
            self.headers = nil;
            self.body = nil;
    }
    return self;

}
- (void) dealloc{}


@end
