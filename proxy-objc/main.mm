//
//  main.m
//  proxy-objc
//
//  Created by ROBERT BLACKWELL on 12/27/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "proxy.hpp"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        RunMitmServer();
    }
    return 0;
}
