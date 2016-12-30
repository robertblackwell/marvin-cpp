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



@interface CapturedTraffic (Delegate)

/**
* NSOutlineView delegate methods
*/

- (NSView *)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item;

/// returns the cell to display
- (NSCell *)outlineView:(NSOutlineView *)outlineView
 dataCellForTableColumn:(NSTableColumn *)tableColumn 
                   item:(id)item;

/// if true shows the expand triangle
- (BOOL)         outlineView:(NSOutlineView *)outlineView
shouldShowOutlineCellForItem:(id)item;

- (BOOL) outlineView:(NSOutlineView*) outlineView
    shouldExpandItem:(id)item;
// asks the delegate whether it’s okay to expand the specified item.

- (BOOL) outlineView:(NSOutlineView*) outlineView
    shouldCollapseItem:(id)item;
// asks the delegate whether it’s okay to collapse the specified item.

- (BOOL) outlineView:(NSOutlineView*)outlineview shouldEditTableColumn:(NSTableColumn*) tableColumn
                item:(id)item;
// - answer no


- (BOOL)outlineView:(NSOutlineView *)outlineView 
   shouldSelectItem:(id)item;

- (void)outlineViewSelectionDidChange:(NSNotification *)notification;

- (void)outlineView:(NSOutlineView *)outlineView 
willDisplayOutlineCell:(id)cell 
     forTableColumn:(NSTableColumn *)tableColumn 
               item:(id)item;

@end
