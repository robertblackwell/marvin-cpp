
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "server.hpp"
#include "request_handler_interface.hpp"
#include "request.hpp"

class MyHandler : public RequestHandlerInterface
{
public:
    MyHandler()
    {
    }
    void handleRequest(
        boost::asio::io_service& io,
        MessageReader& req,
        MessageWriter& resp,
        HandlerDoneCallbackType done
    ){
        std::cout << "got to a new handler " << std::endl;
        std::string u = req.uri();
        std::ostringstream os;
        os << "<!DOCTYPE html>";
        os << "<html><head></head><body>";
        os << "<p>Method: " << req.getMethodAsString() << "</p>";
        os << "<p>Vers Maj: " << req.httpVersMajor() << "</p>";
        os << "<p>Vers Minor: " << req.httpVersMinor() << "</p>";
        req.dumpHeaders(os);
        for(int i = 0; i < 10; i++ ){
            os << "<p>This is simple a line to fill out the transmission</p>";
        }
        os << "</body>";
        
        resp.setStatusCode(200);
        resp.setStatus("OK");
        resp.setHttpVersMajor(1);
        resp.setHttpVersMinor(1);
        std::string s = os.str();
        resp.setContent(s);
        resp.setHeader("Content-length", std::to_string(s.length() ));
        resp.setHeader("Connection","close");
        resp.asyncWrite([done](Marvin::ErrorType& err){;
            done(true);
        });
    }
};

void
runAServerProdItWithBrowser()
{
    try
    {
        Server<MyHandler> server;
        server.listen();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
}

//
// This is the type callback is OK the best when the continued existence of Request is
// guarenteed by something other than the bind to the callback
//
std::shared_ptr<Request> do_request1(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));
    Request& reqRef = *req;
    
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
    req->setMethod(HttpMethod::GET);
    req->setUrl("http://localhost:9991");
    req->setUri("/");
    std::string b("");
    reqRef.setContent(b);
    req->go([ &reqRef, code](Marvin::ErrorType& ec){
        MessageReader& resp = reqRef.getResponse();
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << &reqRef << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << &reqRef << std::endl;

        std::string t = code+code+code+code+code+code+code+code+code + code;
        bool x = (t == resp.getBody());
        if( x ){ std::cout << "OK";} else {std::cout << "FAIL";};
        std::cout << std::endl;
        LogDebug("");
    });
    std::cout << "exit use: " << req.use_count() << std::endl;;
    return std::move(req);
}



void*
serverFunction(void* arg)
{
    long tid;
    tid = (long)arg;
    printf("Hello World! This is server\n");

    Server<MyHandler> server;
    server.listen();

    return (void*)0;
}

void*
clientFunction(void *threadid)
 {
    boost::asio::io_service io(1);
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me client, thread #%ld!\n", tid);
    std::shared_ptr<Request> r1 = do_request1("1", io);
    io.run();
    return (void*)0;
 }
void twoProcesses()
{
    pid_t parent = getpid();
    pid_t pid = fork();
    bool make_server_new_process = true;

    if (pid == -1){
        // error, failed to fork()
    }else if (pid > 0){
        int status;
        sleep(5);
        if( make_server_new_process ){
            clientFunction((void*)2);
            waitpid(pid, &status, 0);
        } else {
            char* arg[] = {(char*)"./client_alone" };
            execv("./client_alone", arg);
            _exit(EXIT_FAILURE);   // exec never returns
        }
    }else{
        if(make_server_new_process ){
            char* arg[] = {(char*)"./server_alone"};
            execv("./server_alone", arg);
            _exit(EXIT_FAILURE);   // exec never returns
        }else{
            serverFunction((void*)"");
        }
    }
}

#define NUMTHREADS 1
int serverAndRequests()
{
    pthread_t serverThread;
    int s_rc = pthread_create(&serverThread, NULL, serverFunction, (void*)(0));

    pthread_t threads[NUMTHREADS];
    int rc;
    long t;
    for(t=0; t<NUMTHREADS; t++){
       printf("In main: creating thread %ld\n", t);
       rc = pthread_create(&threads[t], NULL, clientFunction, (void *)t);
       if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
   }
   long retCode;
   void* rcodePtr = &retCode;;
   for(int i=0 ;i < NUMTHREADS ; i++){
    pthread_join(threads[i], &rcodePtr );
   }
   pthread_join(serverThread, &rcodePtr);
   printf("after joins \r");
}

/* Last thing that main() should do */
pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    std::cout.setf(std::ios::unitbuf);
//    twoProcesses();
    serverAndRequests();
    return 0;
}
