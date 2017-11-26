//
//  CapturedTraffic.h
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "ViewController.h"

#import "http_notification.h"
#import "SingleTransaction.h"
#import "TrafficForHost.h"



@interface CapturedTraffic : NSObject
@property (weak, nonatomic) ViewController* viewController;

- (id)init;
- (NSInteger)       numberOfHosts;
- (TrafficForHost*) hostAtIndex:(NSInteger) index;
- (NSInteger)       indexForHost:(NSString*) host;
- (TrafficForHost*) forHost:(NSString*) host;
- (bool)            hasHost:(NSString*) host;
- (TrafficForHost*) addHost:(NSString*) host;
- (void)            addTrafficForHost:(NSString*) host transaction:(SingleTransaction*) aTRansaction;
- (void)            clear;

@end
