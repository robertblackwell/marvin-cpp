#include <iostream>
#include <fstream>
#include <sstream>

#include "rb_logger.hpp"
#define RBLOGGER_ON

RBLOGGER_SETLEVEL(LOG_LEVEL_INFO)

int main()
{
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
