//
//  ViewController.m
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "ViewController.h"
#import "CapturedTraffic.h"

@interface ViewController ()

@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"viewController");
    // Do view setup here.
}

@end

/*
-(id) outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
 {
    return (item == nil) ? @"/" : [item relativePath];
}

test the class of an object

[myObject isKindOfClass:[NSString class]]

Delegate

- (NSCell *)outlineView:(NSOutlineView *)outlineView 
 dataCellForTableColumn:(NSTableColumn *)tableColumn 
                   item:(id)item;


outlineView:shouldEditTableColumn:item: - answer no

outlineView:willDisplayOutlineCell:forTableColumn:item: - actualdisplaying

outlineView:shouldExpandItem: asks the delegate whether it’s okay to expand the specified item.
outlineViewItemWillExpand: informs the delegate that the outline view is about to expand the specified item.
outlineView:shouldCollapseItem: asks the delegate whether it’s okay to collapse the specified item.
outlineViewItemWillCollapse:
*/
