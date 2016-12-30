//
//  ViewController.h
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/29/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class CapturedTraffic;

@interface ViewController : NSViewController

@property (weak, nonatomic)   IBOutlet NSOutlineView*   outlineView;
@property (strong, nonatomic) IBOutlet NSTextView*      requestView;
@property (strong, nonatomic) IBOutlet NSTextView*      responseView;
@property  CapturedTraffic*    capturedTraffic;

-(void) updateView;
- (void)awakeFromNib;
- (void)doubleClick:(id)object;

@end
