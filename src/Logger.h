#ifndef LOGGER_INC

    #define ELPP_THREAD_SAFE 1

    #include "easylogging++.h"

    #define LOGGER_INC
    #define LOGGER_INIT

    #ifndef LOGGER_INIT

        INITIALIZE_EASYLOGGINGPP

        #define LOGGER_INIT 1

    #endif

#endif