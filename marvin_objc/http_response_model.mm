//
//  ResponseModel.m
//  Broker
//
//  Created by Brendon Blackwell on 18/01/14.
//  Copyright (c) 2014 Robert Blackwell. All rights reserved.
//

#import "http_response_model.h"

@implementation HttpResponseModel
- (id) init
{
    if ( self = [super init] ) {
            self.minorVersion = nil;
            self.statusCode = nil;
            self.status = nil;
            self.headers = nil;
            self.body = nil;
    }
    return self;

}
- (void) dealloc{}

@end
