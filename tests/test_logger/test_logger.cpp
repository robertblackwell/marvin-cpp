#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#define RBLOGGER_ON

RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)

int main()
{
    auto ss = __FILE__;
    RBLogging::addTraceFile(__FILE__);
    LogTrace(std::string("this is a string"));
    LogTrace("this is a string");
    LogError("ERROR This is the message", "a string", (int) 33);
    LogWarn("WARN This is the message", "a string", (int) 33);
    LogInfo("INFO This is the message", "a string", (int) 33);
    LogDebug("DEBUG This is the message", "a string", (int) 33);
    LogVerbose("VERBOSE This is the message", "a string", (int) 33);
    LogError("This is the message", "a string", (int) 33);
    VLogError("This is the message", "a string", (int) 33);
    
    FLogError("This is the message %s %d", (char*)"a string", (int) 33);

    RBLogging::setEnabled(false);
    LogTrace(std::string("this is a string"));
    LogTrace("this is a string");
    LogError("ERROR This is the message", "a string", (int) 33);
    LogWarn("WARN This is the message", "a string", (int) 33);
    LogInfo("INFO This is the message", "a string", (int) 33);
    LogDebug("DEBUG This is the message", "a string", (int) 33);
    LogVerbose("VERBOSE This is the message", "a string", (int) 33);
    LogError("This is the message", "a string", (int) 33);
    VLogError("This is the message", "a string", (int) 33);
    
    FLogError("This is the message %s %d", (char*)"a string", (int) 33);

    RBLogging::enableForLevel(LOG_LEVEL_INFO);
    LogTrace(std::string("this is a string"));
    LogTrace("this is a string");
    LogError("ERROR This is the message", "a string", (int) 33);
    LogWarn("WARN This is the message", "a string", (int) 33);
    LogInfo("INFO This is the message", "a string", (int) 33);
    LogDebug("DEBUG This is the message", "a string", (int) 33);
    LogVerbose("VERBOSE This is the message", "a string", (int) 33);
    LogError("This is the message", "a string", (int) 33);
    VLogError("This is the message", "a string", (int) 33);
    
    FLogError("This is the message %s %d", (char*)"a string", (int) 33);

    return 0;
}
