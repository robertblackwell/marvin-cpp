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
{
NSInteger _expandedRow;
}

- (void)awakeFromNib {
    _expandedRow = -1;
    [_outlineView setTarget:self];
    [_outlineView setDoubleAction:@selector(doubleClick:)];
//    [_outlineView setIndentationPerLevel:0.0];
    [_outlineView setAction:@selector(doClick:)];
    [_outlineView setTarget:self];
}

- (void)doubleClick:(id)object {
  // This gets called after following steps 1-3.
  id s = [object subviews];
  NSInteger previousExpandedRow;
  _expandedRow = [_outlineView clickedRow];
  // Do something...
}


-(IBAction) doClick:(id)sender;
{
    if ([_outlineView clickedRow] == -1) {
        [_outlineView deselectAll:nil];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"viewController");
    // Do view setup here.
}

-(void) updateView
{
    [_outlineView reloadItem:nil];
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
