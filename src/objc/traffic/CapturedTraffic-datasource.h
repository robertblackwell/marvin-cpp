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
#import "SingleTransaction.h"
#import "TrafficForHost.h"



@interface CapturedTraffic (DataSource)


/**
* NOutlineView dataSource methods
*/
-(NSInteger) outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item;

-(BOOL) outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item;

-(id)outlineView:(NSOutlineView *)outlineView
            child:(NSInteger)index
           ofItem:(id)item;

-(id)         outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
                   byItem:(id)item;

@end
