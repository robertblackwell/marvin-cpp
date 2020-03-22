//
// The main entry point for Marvin - a mitm proxy for http/https 
//

#import <Foundation/Foundation.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <marvin/boost_stuff.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

#include <marvin/objc_collector.hpp>
#import "marvin_delegate_objc.h"
#import "http_notification.h"


        static bool             _firstTime;
        static ObjcCollector*   _instance;

ObjcCollector::ObjcCollector(boost::asio::io_service& io): _ioLoop(io), _myStrand(io)
{
}
    
ObjcCollector* ObjcCollector::getInstance(boost::asio::io_service& io)
{
    if( _firstTime ){
        _firstTime = false;
        _instance = new ObjcCollector(io);
    }
    return _instance;
}
void ObjcCollector::setDelegate(void* delegate)
{
    _anonDelegate = delegate;
}
/**
** This method actually implements the collect function but run on a dedicated
** strand. Even if this method does IO-wait operations the other thread will
** keep going
**/
void ObjcCollector::postedCollect(
            std::string& scheme,
            std::string& host,
            MessageReaderSPtr req,
            MessageWriterSPtr resp)
{
    
    /**
    ** Here implement the transmission of any data using sync or async IO
    **/
    HttpNotification* notification = [[HttpNotification alloc]init];
    notification.host = [NSString stringWithUTF8String:host.c_str()];
    notification.scheme= [NSString stringWithUTF8String:scheme.c_str()];
    
    std::string req_uri = req->uri();
    notification.request.uri = [NSString stringWithUTF8String:req_uri.c_str()];

    int req_m = (int)req->method();
    notification.request.method = [NSNumber numberWithInt:req_m];

    std::string req_mstr = req->getMethodAsString();
    const char* tmp_c_str = (req_mstr.c_str());
    NSString* tmpS = [NSString stringWithUTF8String:tmp_c_str];
    notification.request.methodStr =
        [NSString stringWithString:[NSString stringWithUTF8String:req_mstr.c_str()]];

    int req_minorVers = req->httpVersMinor();
    notification.request.minorVersion = [NSNumber numberWithInt:req_minorVers];
    
    int resp_minorVers = resp->httpVersMinor();
    notification.request.minorVersion = [NSNumber numberWithInt:resp_minorVers];
    
    int resp_statusCode = resp->statusCode();
    notification.response.statusCode = [NSNumber numberWithInt:resp_statusCode];
    std::string resp_status = resp->status();
    notification.response.status = [NSString stringWithUTF8String:resp_status.c_str()];
    
    /**
    ** Now send the summarized transaction to OBJC app
    **/
    MarvinDelegateObjc* delegate = (__bridge MarvinDelegateObjc*)_anonDelegate;
    [delegate performSelectorOnMainThread:@selector(notify:) withObject:notification waitUntilDone:NO];

}
/**
** Interface method for client code to call collect
**/
void ObjcCollector::collect(
            std::string& scheme,
            std::string& host,
            MessageReaderSPtr req,
            MessageWriterSPtr resp)
{
    std::cout << (char*)__FILE__ << ":" << (char*) __FUNCTION__ << std::endl;

    /**
    ** In here implement the creation the summary records but dont do any IO or sending
    ** leave that for postedCollect
    **/

    auto pf = _myStrand.wrap(std::bind(&ObjcCollector::postedCollect, this, scheme, host, req, resp));
    _ioLoop.post(pf);
}
    
bool ObjcCollector::_firstTime = true;
ObjcCollector* ObjcCollector::_instance = NULL;
void* ObjcCollector::_anonDelegate = NULL;


