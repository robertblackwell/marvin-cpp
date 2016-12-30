//
//  CapturedTraffic.m
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "CapturedTraffic.h"
#import "CapturedTraffic-delegate.h"

@implementation CapturedTraffic(Delegate)

/// returns the cell to display
- (NSCell *)outlineView:(NSOutlineView *)outlineView
 dataCellForTableColumn:(NSTableColumn *)tableColumn 
                   item:(id)item
{
    NSTableCellView* v1 = (NSTableCellView*)[outlineView makeViewWithIdentifier:@"" owner:self];
    NSInteger c = 0;
    NSInteger r = 1;
    NSTableCellView* v2 = (NSTableCellView*)[outlineView viewAtColumn:c row: r makeIfNecessary:YES];
    return (NSCell*)v2;
}

/// if true shows the expand triangle
- (BOOL)         outlineView:(NSOutlineView *)outlineView
shouldShowOutlineCellForItem:(id)item
{
    return [item isKindOfClass:[TrafficForHost class]];

}

// asks the delegate whether it’s okay to expand the specified item.
- (BOOL) outlineView:(NSOutlineView*) outlineView
    shouldExpandItem:(id)item
{
    return [item isKindOfClass:[TrafficForHost class]];
}

// asks the delegate whether it’s okay to collapse the specified item.
- (BOOL) outlineView:(NSOutlineView*) outlineView
    shouldCollapseItem:(id)item
{
    return [item isKindOfClass:[TrafficForHost class]];
}

// - answer no
- (BOOL) outlineView:(NSOutlineView*)outlineview shouldEditTableColumn:(NSTableColumn*) tableColumn
                item:(id)item
{
    return NO;
}


- (BOOL)outlineView:(NSOutlineView *)outlineView 
   shouldSelectItem:(id)item
{
    return YES;
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
    NSLog(@"selectioin changed");
}

- (void)outlineView:(NSOutlineView *)outlineView 
willDisplayOutlineCell:(id)cell 
     forTableColumn:(NSTableColumn *)tableColumn 
               item:(id)item
{
    NSLog(@"displaying cell");
}




@end
