
#import "http_notification.h"
#import "http_request_model.h"
#import "http_response_model.h"

@implementation HttpNotification

- (id) init
{
    if ( self = [super init] ) {
            self.requestId = nil;
            self.host = nil;
            self.scheme = nil;
            self.request = [[HttpRequestModel alloc]init];
            self.response = [[HttpResponseModel alloc]init];
    }
    return self;
}

- (void) dealloc{
}

/**
 *  This is the method the defines what is printed out
 *  when you type "po object" into the consoler
 *
 *  @return String representation of model
 */
-(NSString*)description
{
    return [NSString stringWithFormat:@"%@",self.host];
}

@end
