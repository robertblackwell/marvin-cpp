#import <Foundation/Foundation.h>

@class CapturedTraffic;

/**
* This is an example class, that shows how to get notification of http transactions
* from the Marvin proxy
*/
@interface MarvinDelegateObjc : NSObject
- (id) init;
- (void) setCapturedTraffic:(CapturedTraffic*) cTraffic;
- (void) notify:(id)data;
@end