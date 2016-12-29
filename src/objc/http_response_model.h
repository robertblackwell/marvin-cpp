//
//  ResponseModel.h
//  Broker
//
//  Created by Brendon Blackwell on 18/01/14.
//  Copyright (c) 2014 Robert Blackwell. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HttpResponseModel : NSObject

@property (strong, nonatomic) NSNumber* minorVersion;
@property (strong, nonatomic) NSNumber* statusCode;
@property (strong, nonatomic) NSString* status;
@property (strong, nonatomic) NSString* headers;
@property (strong, nonatomic) NSString* body;

- (id) init;
- (void) dealloc;
 
@end
