//
//  CapturedTraffic.m
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import "CapturedTraffic.h"
#import "CapturedTraffic-datasource.h"


//+ (CapturedTraffic*)sharedInstance;

@implementation CapturedTraffic(DataSource)

/// These are dataSource methods for NSOutlineView
-(NSInteger) outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
    NSInteger ans;
    if( [item isKindOfClass:[CapturedTraffic class]] || (item == nil) ){
        ans = [self numberOfHosts];
    }else if( [item isKindOfClass:[TrafficForHost class]] ){
        ans = [[item transactions] count];
    }else if( [item isKindOfClass:[SingleTransaction class]] ){
        ans = 0;
    }else{
        NSLog(@"TROUBLE");
        ans = -1;
    }
    return ans;
}
-(BOOL) outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    BOOL ans = (
        [item isKindOfClass:[CapturedTraffic class]] ||
        [item isKindOfClass:[TrafficForHost class]]
    );
    return ans;
}

- (id)outlineView:(NSOutlineView *)outlineView
            child:(NSInteger)index
           ofItem:(id)item
{
    id ans;
    if( [item isKindOfClass:[CapturedTraffic class]] || (item == nil) ){
        ans = [self hostAtIndex:index];
    }else if( [item isKindOfClass:[TrafficForHost class]] ){
        if( [[item transactions] count] > index ){
            ans = [[item transactions] objectAtIndex:index];
        }
    }else if( [item isKindOfClass:[SingleTransaction class]] ){
        ans = nil;
    }else{
        NSLog(@"TROUBLE");
        ans = nil;
    }
    return ans;
}

-(id)         outlineView:(NSOutlineView *)outlineView
objectValueForTableColumn:(NSTableColumn *)tableColumn
                   byItem:(id)item
{
    return item;
}
@end
