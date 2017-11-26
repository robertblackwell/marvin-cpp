//
//  RequestModel.h
//  Broker
//
//  Created by Brendon Blackwell on 18/01/14.
//  Copyright (c) 2014 Robert Blackwell. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HttpRequestModel : NSObject

@property (strong, nonatomic) NSNumber* minorVersion;
@property (strong, nonatomic) NSNumber* method;
@property (strong, nonatomic) NSString* methodStr;
@property (strong, nonatomic) NSString* uri;
@property (strong, nonatomic) NSString* headers;
@property (strong, nonatomic) NSString* body;

- (id) init;
- (void) dealloc;

@end
