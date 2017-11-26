
#import <Foundation/Foundation.h>
#import "http_request_model.h"
#import "http_response_model.h"

@interface HttpNotification : NSObject

@property (strong, nonatomic) NSString* requestId;
@property (strong, nonatomic) NSString* host;
@property (strong, nonatomic) NSString* scheme;

@property (strong, nonatomic) HttpRequestModel*     request;
@property (strong, nonatomic) HttpResponseModel*    response;

- (id) init;
- (void) dealloc;

@end

