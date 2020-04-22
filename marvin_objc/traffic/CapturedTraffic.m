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
    static id _instance;
    if ( self = [super init] ) {
        _instance = self;
        _hosts = [[NSMutableDictionary alloc]init];
        _hostsArray = [[NSMutableArray alloc]init];
//        SingleTransaction* t1 = [[SingleTransaction alloc]init];
//        SingleTransaction* t2 = [[SingleTransaction alloc]init];
//        [self addTrafficForHost:@"myhost" transaction:t1 ];
//        [self addTrafficForHost:@"myhost" transaction:t2];
    }
    return self;
}

- (void) dealloc
{
}

-(NSInteger) numberOfHosts
{
    return [_hostsArray count];
}
- (TrafficForHost*) hostAtIndex:(NSInteger) index
{
    NSString* hn = [_hostsArray objectAtIndex:index];
    return [_hosts objectForKey:hn];
}

- (NSInteger) indexForHost:(NSString*) host
{
    if( [_hosts objectForKey:host] == nil )
        return -1;
    for( int i = 0; i < [_hostsArray count]; i++ ){
        if( [host isEqualToString:[_hostsArray objectAtIndex:i]] ){
            return i;
        }
    }
    return -1;
}

- (bool) hasHost:(NSString*) host
{
    return (nil != [_hosts objectForKey:host]);
}

- (TrafficForHost*) addHost:(NSString*) host
{
    id h = [_hosts objectForKey:host];
    if( h == nil ){
        TrafficForHost* tmp = [[TrafficForHost alloc]initForHost:host];
        [_hosts setObject:tmp forKey:host];
        [_hostsArray addObject:host];
    }else{
        /// nothing
    }
    return [_hosts objectForKey:host];
}
- (void) addTrafficForHost:(NSString*) host transaction:(SingleTransaction*) aTransaction
{
    [[self addHost:host] add:aTransaction];
    [_viewController updateView];
}

- (void) clear
{
    _hosts = [[NSMutableDictionary alloc]init];
}


@end
