#import "marvin_delegate_objc.h"
#import "http_notification.h"
#import "CapturedTraffic.h"

@implementation MarvinDelegateObjc
{
    CapturedTraffic* _capturedTraffic;
}

- (id) init
{
    if ( self = [super init] ) {
    }
    return self;
}

-(void) setCapturedTraffic:(CapturedTraffic*) cTraffic
{
    _capturedTraffic = cTraffic;
}

- (void) notify:(id) notification
{
    HttpNotification* n = (HttpNotification*)notification;
    SingleTransaction* t = (SingleTransaction*) n;
    [_capturedTraffic addTrafficForHost:t.host transaction:t];
    
    NSLog(@" Delegate request.method %@", n.request.methodStr );
    NSLog(@" Delegate request.uri %@", n.request.uri );
    NSLog(@" Delegate request.mVers %@", n.request.minorVersion );
    
    NSLog(@" Delegate response.statusCode %@", n.response.statusCode );
    NSLog(@" Delegate response.Status %@", n.response.status );
    NSLog(@" Delegate mVers %@", n.response.minorVersion );
}

@end