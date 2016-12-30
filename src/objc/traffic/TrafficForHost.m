//
//  CapturedTraffic.m
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "TrafficForHost.h"



@implementation TrafficForHost
{
    NSMutableArray* _traffic;
}
- (id) initForHost:(NSString*) aHost
{
    if ( self = [super init] ) {
        _hostName = aHost;
        _traffic = [[NSMutableArray alloc]init];
    }
    return self;
}

- (NSArray*) transactions
{
    return _traffic;
}

- (void) add:(SingleTransaction*) transaction
{
    [_traffic addObject:transaction];
}
@end

