//
//  CapturedTraffic.m
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "CapturedTraffic.h"


//+ (CapturedTraffic*)sharedInstance;

@implementation CapturedTraffic
{
    NSMutableDictionary*    _hosts;
    NSMutableArray*         _hostsArray;
}

- (id) init{
    if ( self = [super init] ) {
        _hosts = [[NSMutableDictionary alloc]init];
        SingleTransaction* t1 = [[SingleTransaction alloc]init];
        SingleTransaction* t2 = [[SingleTransaction alloc]init];
        [self addTrafficForHost:@"myhost" transaction:t1 ];
        [self addTrafficForHost:@"myhost" transaction:t2];
    }
    return self;
}

- (void) dealloc
{
}

-(NSInteger) numberOfHosts
{
    return [_hostArray count];
}
- (TrafficForHost*) hostAtIndex:(NSInteger) index
{
    return [_hostsArray objectAtIndex:index];
}

- (bool) hasHost:(NSString*) host
{
    return (nil != [_hosts objectForKey:host]);
}

- (TrafficForHost*) addHost:(NSString*) host
{
    id h = [_hosts objectForKey:host];
    if( h == nil ){
        TrafficForHost* tmp = [[TrafficForHost alloc]init];
        [_hosts setObject:tmp forKey:host];
    }else{
        /// nothing
    }
    return [_hosts objectForKey:host];
}
- (void) addTrafficForHost:(NSString*) host transaction:(SingleTransaction*) aTransaction
{
    [[self addHost:host] add:aTransaction];
}

- (void) clear
{
    _hosts = [[NSMutableDictionary alloc]init];
}


@end
