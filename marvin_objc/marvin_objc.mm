//
//  AppDelegate.m
//  proxy_cocoa
//
//  Created by ROBERT BLACKWELL on 12/28/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "marvin_objc.h"

#include <marvin/server/http_server.hpp>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/client/request.hpp>
#include <marvin/forwarding/forwarding_handler.hpp>
#include <marvin/objc_collector.hpp>

#import "marvin_delegate_objc.h"

void RunMitmServer()
{
    
    MarvinDelegateObjc *delegate = [[MarvinDelegateObjc alloc]init];
    ObjcCollector::setDelegate((__bridge void*)delegate);
    
    HTTPServer<ForwardingHandler<ObjcCollector>> server;
    server.listen(9991);
    NSLog(@"Done with RunMitmServer");
}
@interface MarvinObj:NSObject
- (void) run;
@end

@implementation MarvinObjc
{
    id          _delegateObj;
    long        _port;
}

- (void) run
{
    ObjcCollector::setDelegate((__bridge void*)_delegateObj);
    HTTPServer<ForwardingHandler<ObjcCollector>> server;
    server.listen(9991);
    NSLog(@"Done with RunMitmServer");
}

/**
* Starts the Marvin proxy on a background thread so that boost can manage its own run loop
*/
- (void)start
{
    [self performSelectorInBackground:@selector(run) withObject:self];
}

- (void) setDelegate:(id) delegateObj { _delegateObj = delegateObj; }
- (void) setPort:(long)port { _port = port; }
@end

