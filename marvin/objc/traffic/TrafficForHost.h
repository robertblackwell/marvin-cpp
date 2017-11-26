//
//  CapturedTraffic.h
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "http_notification.h"

@class SingleTransaction;


@interface TrafficForHost : NSObject
@property (weak, nonatomic) NSString* hostName;

- (id) initForHost:(NSString*) aHost;
- (NSArray*) transactions;
- (void)     add:(SingleTransaction*) transaction;
@end

