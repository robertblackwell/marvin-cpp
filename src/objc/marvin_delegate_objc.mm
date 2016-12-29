#import "marvin_delegate_objc.h"
#import "http_notification.h"

@implementation MarvinDelegateObjc

- (void) notify:(id) notification
{
    HttpNotification* n = (HttpNotification*)notification;
    NSLog(@" Delegate request.method %@", n.request.methodStr );
    NSLog(@" Delegate request.uri %@", n.request.uri );
    NSLog(@" Delegate request.mVers %@", n.request.minorVersion );
    
    NSLog(@" Delegate response.statusCode %@", n.response.statusCode );
    NSLog(@" Delegate response.Status %@", n.response.status );
    NSLog(@" Delegate mVers %@", n.response.minorVersion );
}

@end