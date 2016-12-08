//
//
//
//
//
//
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#ifndef RBLOGGER_HPP
#define RBLOGGER_HPP
namespace RBLogging{
 
enum class LogLevel {
    error = 1,
    warn = 2,
    info = 3,
    debug = 4,
    verbose = 5
};

    
#ifdef CCCCC // 'C' or 'C++'
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4
#define LOG_LEVEL_VERBOSE   5
    
    typedef long LogLevelType
    
#else
    
#define LOG_LEVEL_ERROR     RBLogging::LogLevel::error
#define LOG_LEVEL_WARN      RBLogging::LogLevel::warn
#define LOG_LEVEL_INFO      RBLogging::LogLevel::info
#define LOG_LEVEL_DEBUG     RBLogging::LogLevel::debug
#define LOG_LEVEL_VERBOSE   RBLogging::LogLevel::verbose
    
    typedef LogLevel LogLevelType;
    
#endif

std::string LogLevelText(LogLevelType level);

class Logger{
public:
    Logger(std::ostream& os = std::cerr): __outStream(os){
    }
    void logWithFormat(
                       LogLevelType level,
                       LogLevelType threshold,
                       const char* file_name,
                       const char* func_name,
                       int line_number,
                       char* format,
                       ...);
    template<typename T, typename... Types>
    void vlog(
              LogLevelType level,
              LogLevelType threshold,
              const char* file_name,
              const char* func_name,
              int line_number,
              const T& firstArg,
              const Types&... args)
    {
        std::ostringstream os;
        if( levelIsActive(level, threshold) ){
            os << LogLevelText(level) <<"|";
            auto tmp2 = boost::filesystem::path(file_name);
            auto tmp3 = tmp2.filename();
            auto tmp4 = tmp3.stem();

            os <<  tmp3.c_str();
            os << "::"<< func_name << "[" << line_number << "]:";
            myprint(os, firstArg, args...);
            //
            // Only use the stream in the last step and this way we can send the log record somewhere else
            // easily
            //
            __outStream << os.str();
        }
    }
    
private:
    std::ostream& __outStream;
    std::string className(std::string& func_name);
    bool levelIsActive(LogLevelType lvl, LogLevelType threshold){
        return ((int)lvl <= (int)threshold);
    }
    template <typename T, typename... Types>
    void myprint (std::ostringstream& os,  const T& firstArg, const Types&... args)
    {
        os << " " << firstArg ; // print first argument
        myprint(os, args...); // call print() for remaining arguments
    }

    void myprint(std::ostringstream& os)
    {
        os << std::endl;
    }
};


// want to default to "ON" - disable by #define RBLOGGER_OFF
#if !defined(RBLOGGER_OFF) || defined(RBLOGGER_ON) || defined(RBLOGGER_ENABLED)
    #define RBLOGGER_ENABLED
#else
    #undef RBLOGGER_ENABLED
#endif
    
#if ! defined(RBLOGGER_ENABLED)
    
    #define ROBMACROFormatLog(lvl, frmt, ...)
    #define ROBMACROLog(lvl, arg1, ...)

#else
    #define ROBMACROFormatLog(lvl, frmt, ...) \
        RBLogging::activeLogger.logWithFormat(\
            /*log:*/        lvl, \
            /*threshold:*/  rbLogLevel, \
            /*file */       (char*)__FILE__, \
            /*function:*/   (char*)__FUNCTION__, \
            /*line:*/       __LINE__, \
            /*format:*/     ((char*)frmt), \
            /*var args*/    ##__VA_ARGS__\
            )

    #define ROBMACROLog(lvl, arg1, ...) \
        RBLogging::activeLogger.vlog(\
            /*log:*/        lvl, \
            /*threshold:*/  rbLogLevel, \
            /*file */       (char*)__FILE__, \
            /*function:*/   (char*)__FUNCTION__, \
            /*line:*/       __LINE__, \
            /*arg1*/        arg1, \
            /*var args*/    ##__VA_ARGS__\
        )
#endif
//
// define the printf style log macros
//
#define  FLogError(frmt, ...)   ROBMACROFormatLog(LOG_LEVEL_ERROR, frmt, ##__VA_ARGS__)
#define  FLogWarn(frmt, ...)    ROBMACROFormatLog(LOG_LEVEL_WARN, frmt, ##__VA_ARGS__)
#define  FLogInfo(frmt, ...)    ROBMACROFormatLog(LOG_LEVEL_INFO, frmt, ##__VA_ARGS__)
#define  FLogDebug(frmt, ...)   ROBMACROFormatLog(LOG_LEVEL_DEBUG, frmt, ##__VA_ARGS__)
#define  FLogVerbose(frmt, ...) ROBMACROFormatLog(LOG_LEVEL_VERBOSE, frmt, ##__VA_ARGS__)

//
// define the NON printf style log macros - simply a list of things to print
//
#define  VLogError(arg1, ...)   ROBMACROLog(LOG_LEVEL_ERROR,   arg1, ##__VA_ARGS__)
#define  VLogWarn(arg1, ...)    ROBMACROLog(LOG_LEVEL_WARN,    arg1, ##__VA_ARGS__)
#define  VLogInfo(arg1, ...)    ROBMACROLog(LOG_LEVEL_INFO,    arg1, ##__VA_ARGS__)
#define  VLogDebug(arg1, ...)   ROBMACROLog(LOG_LEVEL_DEBUG,   arg1, ##__VA_ARGS__)
#define  VLogVerbose(arg1, ...) ROBMACROLog(LOG_LEVEL_VERBOSE, arg1, ##__VA_ARGS__)

#define  LogError(arg1, ...)   ROBMACROLog(LOG_LEVEL_ERROR,   arg1, ##__VA_ARGS__)
#define  LogWarn(arg1, ...)    ROBMACROLog(LOG_LEVEL_WARN,    arg1, ##__VA_ARGS__)
#define  LogInfo(arg1, ...)    ROBMACROLog(LOG_LEVEL_INFO,    arg1, ##__VA_ARGS__)
#define  LogDebug(arg1, ...)   ROBMACROLog(LOG_LEVEL_DEBUG,   arg1, ##__VA_ARGS__)
#define  LogVerbose(arg1, ...) ROBMACROLog(LOG_LEVEL_VERBOSE, arg1, ##__VA_ARGS__)

    

static Logger activeLogger{};


} // namespace RBLogging

#endif // header guard

#define SET_LOGLEVEL(level) static RBLogging::LogLevelType rbLogLevel = level;

#define RBLOGGER_SETLEVEL(level) static  RBLogging::LogLevelType rbLogLevel = level;
