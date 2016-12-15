
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <boost/asio.hpp>
#include <pthread.h>
#include "rb_logger.hpp"
RBLOGGER_SETLEVEL(LOG_LEVEL_DEBUG)

#include "server.hpp"
#include "handler_interface.hpp"
#include "request.hpp"
#include "test_client.hpp"
#include "test_server.hpp"



void*
serverFunction(void* arg)
{
    long tid;
    tid = (long)arg;
    printf("Hello World! This is server\n");
    runTestServer();

    return (void*)0;
}

void*
clientFunction(void *threadid)
 {
    boost::asio::io_service io(1);
    long tid;
    tid = (long)threadid;
    LogDebug("Hello World! It's me client");
    runTestClient();
    io.run();
    return (void*)0;
 }
//---------------------------------------------------------------------------------------
//
// Run cient and server in separate processes
//
//---------------------------------------------------------------------------------------
void twoProcesses()
{
    pid_t parent = getpid();
    pid_t child_pid = fork();
    LogDebug("starting two processes");
    std::cout.setf(std::ios::unitbuf);
    if (child_pid == -1){
        // error, failed to fork()
    }else if (child_pid > 0){
        std::cout.setf(std::ios::unitbuf);
        // this is the child process
        int status;
        
        sleep(5);
        LogDebug("about to start client");
        clientFunction((void*)2);
        kill(child_pid, 9);
        waitpid(child_pid, &status, 0);
    }else{
        std::cout.setf(std::ios::unitbuf);
        LogDebug("about to fork server");
        char* arg[] = {(char*)"./server_alone"};
        execv("./server_alone", arg);
        _exit(EXIT_FAILURE);   // exec never returns
    }
}
//---------------------------------------------------------------------------------------
// run client and server in separate P_THREADS
//---------------------------------------------------------------------------------------
#define NUMTHREADS 1
int serverAndRequests()
{
    std::cout.setf(std::ios::unitbuf);

    pthread_t serverThread;
    int s_rc = pthread_create(&serverThread, NULL, serverFunction, (void*)(0));
    
    pthread_t threads[NUMTHREADS];
    int rc;
    long t;
    for(t=0; t<NUMTHREADS; t++){
       LogDebug("In main: creating thread %ld\n", t);
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
//    twoProcesses();
    serverAndRequests();
    return 0;
}
