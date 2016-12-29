//
//  AppDelegate.m
//  proxy_cocoa
//
//  Created by ROBERT BLACKWELL on 12/28/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "AppDelegate.h"

#include "server.hpp"
#include "request_handler_base.hpp"
#include "request.hpp"
#include "forwarding_handler.hpp"
#include "pipe_collector.hpp"

#import "marvin_objc.h"
#import "marvin_delegate_objc.h" //this is only a simple minded sample - provide a real one


@implementation AppDelegate
{
    MarvinObjc*         mitm;
    MarvinDelegateObjc* marvinDelegate;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    /// When the rest of the app is started fire off the proxi
    mitm = [[MarvinObjc alloc]init];
    marvinDelegate = [[MarvinDelegateObjc alloc]init];
    
    [mitm setPort:9991];
    
    // .... there will be othere config parameters in time
    
    [mitm setDelegate:marvinDelegate];
    [mitm start];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

@end
