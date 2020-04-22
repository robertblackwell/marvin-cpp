#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <marvin/configure_trog.hpp>
#define RBLOGGER_ON

TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

int main()
{
    auto ss = __FILE__;
    RBLogging::addTraceFile(__FILE__);
    TROG_TRACE3((std::string("this is a string"));
    TROG_TRACE3(("this is a string");
    TROG_ERROR("ERROR This is the message", "a string", (int) 33);
    TROG_WARN("WARN This is the message", "a string", (int) 33);
    TROG_INFO("INFO This is the message", "a string", (int) 33);
    TROG_DEBUG("DEBUG This is the message", "a string", (int) 33);
    TROG_VERBOSE("VERBOSE This is the message", "a string", (int) 33);
    TROG_ERROR("This is the message", "a string", (int) 33);
    VTROG_ERROR("This is the message", "a string", (int) 33);
    
    FTROG_ERROR("This is the message %s %d", (char*)"a string", (int) 33);

    RBLogging::setEnabled(false);
    TROG_TRACE3((std::string("this is a string"));
    TROG_TRACE3(("this is a string");
    TROG_ERROR("ERROR This is the message", "a string", (int) 33);
    TROG_WARN("WARN This is the message", "a string", (int) 33);
    TROG_INFO("INFO This is the message", "a string", (int) 33);
    TROG_DEBUG("DEBUG This is the message", "a string", (int) 33);
    TROG_VERBOSE("VERBOSE This is the message", "a string", (int) 33);
    TROG_ERROR("This is the message", "a string", (int) 33);
    VTROG_ERROR("This is the message", "a string", (int) 33);
    
    FTROG_ERROR("This is the message %s %d", (char*)"a string", (int) 33);

    RBLogging::enableForLevel(LOG_LEVEL_INFO);
    TROG_TRACE3((std::string("this is a string"));
    TROG_TRACE3(("this is a string");
    TROG_ERROR("ERROR This is the message", "a string", (int) 33);
    TROG_WARN("WARN This is the message", "a string", (int) 33);
    TROG_INFO("INFO This is the message", "a string", (int) 33);
    TROG_DEBUG("DEBUG This is the message", "a string", (int) 33);
    TROG_VERBOSE("VERBOSE This is the message", "a string", (int) 33);
    TROG_ERROR("This is the message", "a string", (int) 33);
    VTROG_ERROR("This is the message", "a string", (int) 33);
    
    FTROG_ERROR("This is the message %s %d", (char*)"a string", (int) 33);

    return 0;
}
