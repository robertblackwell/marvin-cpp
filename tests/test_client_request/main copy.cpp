//
// This file makes multiple parallel requests to the same host and checks that
// each reply corresponds to the request. That is ensures no "mixing up".
//
// Note the requests go to a local private host that has been programmed
// specifically for this test
//
// Primarily this is a test of the Request object and uses Request in it's
// simplest for - that is sending an entire http message in one hit and with
// no body.
//
// Consider running this test with different Connection management strategies
// to see whether we can save on "async_connect" calls and maybe even get some
// pipe-lining
//
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include <marvin/boost_stuff.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)
#include <marvin/http/headers_v2.hpp>
#include <marvin/client/request.hpp>
#include <marvin/client/client.hpp>

#undef VERBOSE
/*
* Now include test cases
*/
#include "pipeline.cpp"
#include "multiple.cpp"
#include "roundtrip.cpp"

class LocalRequest : public Request{
    public:
        ConnectionInterface* getConnection()
        {
            return _connection;
        }
};
//
// This function returns a shared pointer to a Request object. SDomething must hold
// onto that request object or it will get deallocated while the request is still being processed.
//
std::shared_ptr<Request> do_get_request(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));

#ifdef VERBOSE
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
#endif

    req->setMethod(HttpMethod::GET);
    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);

    std::string b("");
    req->setContent(b);
    req->go([ req, code](Marvin::ErrorType& ec){

        MessageReader& resp = req->getResponse();

#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
        std::string t = code+code+code+code+code+code+code+code+code + code;
        
        //
        // Here is where we check the result
        //
        bool x = (t == resp.getBody());
        std::cout << "test for :" << code;
        if( x )
        {
            std::cout << " OK";
        }
        else
        {
            std::cout << " FAIL";
        }
        std::cout << std::endl;
        TROG_DEBUG("");
    });
#ifdef VERBOSE
    std::cout << "exit use: " << req.use_count() << std::endl;
#endif
    return req;
}
void examineRequestsForConnectionReuse(std::vector<std::shared_ptr<Request>> rt)
{
    std::map<long, long> m;
    for( auto const & r: rt) {
        std::cout << std::hex << r.get() << std::endl;
        long r_addr = (long)(r.get());
        LocalRequest* l_req = static_cast<LocalRequest*>(r.get());
        ConnectionInterface* cptr = l_req->getConnection();
        long addr = (long) cptr;
        m[addr] = addr;
    }
    if( m.size() != rt.size() ) {
        std::cout << " some reuse of connections" << std::endl;
    } else {
        std::cout << " no reuse of connections" << std::endl;
    }
}

std::shared_ptr<Client> do_client_connect(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn/utests/echo/test.php"));
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    client->asyncConnect([client, msg, code](Marvin::ErrorType& ec){
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
    });

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}

std::shared_ptr<Client> do_client_roundtrip(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn.com/posts/rtw" ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
//    msg->setIsRequest(true);
//    msg->setHttpVersMajor(1);
//    msg->setHttpVersMinor(1);
//    msg->setUri("/posts/rtw");
    msg->setMethod(HttpMethod::GET);
    
//    msg->setHeader(HttpHeader::Name::Host, "whiteacorn.com");
//    msg->setHeader(HttpHeader::Name::Connection, "Close");
//    msg->setHeader(HttpHeader::Name::ContentLength, "0");

    
    std::function<void(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)> f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = client->getResponse();
        std::string bdy = b->getBody();
        std::cout << bdy << std::endl;
        
#endif
    };
    client->asyncWrite(msg, f);

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}


std::shared_ptr<Client> do_client_rt_fragbuffer(std::string code, boost::asio::io_service& io)

{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn.com/" ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
    msg->setIsRequest(true);
    msg->setHttpVersMajor(1);
    msg->setHttpVersMinor(1);
    msg->setUri("/posts/rtw");
    msg->setMethod(HttpMethod::GET);
    
    msg->setHeader(HttpHeader::Name::Host, "whiteacorn.com");
    msg->setHeader(HttpHeader::Name::Connection, "Close");
    msg->setHeader(HttpHeader::Name::ContentLength, "0");

    
    auto f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = client->getResponse();
        std::string bdy = b->getBody();
        std::cout << bdy << std::endl;
        
#endif
    };
    client->asyncWrite(msg, f);

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}



void testMultipleRequestsInParallelToSameHost()
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Request>> rt;
//        std::shared_ptr<Request> p1 = do_request1("1", io_service);
        //
        // note : must save the return value from do_request1 or else the request object
        // gets pulled out from under us while we are getting the response
        //
#if 1
        auto p1 = testcase_pipeline(io_service);
#endif
#if 0
        auto c1 = do_client_roundtrip("1", io_service);
#endif
#if 0
        rt.push_back(do_get_request("1", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));
        rt.push_back(do_get_request("D", io_service));
        rt.push_back(do_get_request("E", io_service));
        rt.push_back(do_get_request("F", io_service));
        rt.push_back(do_get_request("G", io_service));
#endif
        io_service.run();
        examineRequestsForConnectionReuse(rt);
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
}

int main(){
    testMultipleRequestsInParallelToSameHost();
    return 0;
}
