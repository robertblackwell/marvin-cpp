#import <Foundation/Foundation.h>

/**
* This is an example class, that shows how to get notification of http transactions
* from the Marvin proxy
*/
@interface MarvinDelegateObjc : NSObject
- (void) notify:(id)data;
@end