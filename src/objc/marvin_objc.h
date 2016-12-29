/**
* This class is a wrapper around the c/c++ implementation of the marvin proxy
*
*/
#import <Cocoa/Cocoa.h>

@protocol MarvinDelegate
- (void) notify:(id) data;
@end

@interface MarvinObjc : NSObject
- (void) start;
- (void) setDelegate:(id) delegateObj; //conforms to MarvinDelegate protocol
- (void) setPort:(long) port;
@end


