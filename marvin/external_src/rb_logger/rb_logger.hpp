//
#include <iostream>
#include <sstream>
#include <set>
#include <pthread.h>
#include <boost/filesystem.hpp>
#ifndef RBLOGGER_HPP
#define RBLOGGER_HPP
namespace RBLogging{
#pragma mark - typdefs and constants
enum class LogLevel {
    error = 1,
    warn = 2,
    trace = 3,
    tortrace = 4,
    info = 5,
    debug = 6,
    verbose = 7,
};

    
#ifdef CCCCC // 'C' or 'C++'
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_TRACE     3
#define LOG_LEVEL_TORTRACE  4
#define LOG_LEVEL_INFO      5
#define LOG_LEVEL_DEBUG     6
#define LOG_LEVEL_VERBOSE   7
#define LOG_LEVEL_MAX       7
    
typedef long LogLevelType
    
#else
    
#define LOG_LEVEL_ERROR     RBLogging::LogLevel::error
#define LOG_LEVEL_WARN      RBLogging::LogLevel::warn
#define LOG_LEVEL_TRACE     RBLogging::LogLevel::trace
#define LOG_LEVEL_TORTRACE  RBLogging::LogLevel::tortrace
#define LOG_LEVEL_INFO      RBLogging::LogLevel::info
#define LOG_LEVEL_DEBUG     RBLogging::LogLevel::debug
#define LOG_LEVEL_VERBOSE   RBLogging::LogLevel::verbose
#define LOG_LEVEL_MAX       RBLogging::LogLevel::verbose

    typedef LogLevel LogLevelType;
    using FilePathType = boost::filesystem::path;
    using  FilePathListType = std::set<FilePathType>;

#endif
#pragma mark - globals and functions

extern bool logger_enabled;
extern LogLevelType globalThreshold;
extern FilePathListType activeFileStems;

std::string LogLevelText(LogLevelType level);
void setEnabled(bool on_off);
void enableForLevel(LogLevelType level);
void setActiveFileStems(FilePathListType active_stems );
void addTraceFile(const char* filepath);
void addTraceFile(std::string filepath);


#pragma mark - Logger class
class Logger{
    public:
    
        Logger(std::ostream& os = std::cerr);
    
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
            if( level == LOG_LEVEL_TRACE) {
                assert(true);
            }
            if( levelIsActive(level, threshold) ){
                std::lock_guard<std::mutex> lg(_loggerMutex);
                std::string s = LogLevelText(level);
                os << LogLevelText(level) <<"|";
                auto tmp2 = boost::filesystem::path(file_name);
                auto tmp3 = tmp2.filename();
                auto tmp4 = tmp3.stem();
                auto pid = ::getpid();
                auto tid = pthread_self();


                os <<  tmp3.c_str() << "[" << pid << ":" << tid << "]";
                os << "::"<< func_name << "[" << line_number << "]:";
                myprint(os, firstArg, args...);
                //
                // Only use the stream in the last step and this way we can send the log record somewhere else
                // easily
                //
                write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    //            __outStream << os.str();
            }
        }
        /// \brief custom log template function for LogTrace macro
        template<typename T, typename... Types>
        void tracelog(
//                  LogLevelType level,
//                  LogLevelType threshold,
                  const char* file_name,
                  const char* func_name,
                  int line_number,
                  const T& firstArg,
                  const Types&... args)
        {
            std::ostringstream os;
            boost::filesystem::path file_path = boost::filesystem::path(file_name);
            auto tmp3 = file_path.filename();
            auto tmp4 = tmp3.stem();
            if( p_fileStemIsActive(file_path) ){
                std::lock_guard<std::mutex> lg(_loggerMutex);
//                std::string s = LogLevelText(level);
                os << "TRACE|";
                auto pid = ::getpid();
                auto tid = pthread_self();

                os <<  tmp3.c_str() << "[" << pid << ":" << tid << "]";
                os << "::"<< func_name << "[" << line_number << "]:";
                myprint(os, firstArg, args...);
                //
                // Only use the stream in the last step and this way we can send the log record somewhere else
                // easily
                //
                write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    //            __outStream << os.str();
            }
        }
        void torTraceLog(
                  const char* file_name,
                  const char* func_name,
                  int line_number,
                  void* this_arg);

        void fdTraceLog(
                  const char* file_name,
                  const char* func_name,
                  int line_number,
                  long fd_arg);
    
    private:
        std::mutex _loggerMutex;

        std::ostream& m_outStream;
        std::vector<boost::filesystem::path> active_stems;

        std::string p_className(std::string& func_name);
        bool enabled();
        bool levelIsActive(LogLevelType lvl, LogLevelType threshold);
        bool p_fileStemIsActive(FilePathType stem);
        void myprint(std::ostringstream& os);

        template <typename T, typename... Types>
        void myprint (std::ostringstream& os,  const T& firstArg, const Types&... args)
        {
            os << " " << firstArg ;
            myprint(os, args...);
        }

};
#pragma mark - dclare a logger
#undef LOGGER_SINGLE
#ifdef LOGGER_SINGLE
extern Logger activeLogger;
#else
static Logger activeLogger{};
#endif

#pragma mark - macros
// want to default to "ON" - disable Log by #define RBLOGGER_OFF
//#define RBLOGGER_OFF

///
/// These trun off/on tracing of special attributes
///

#define NO_TRACE          // trace key points in the message traffic
#define NO_CTORTRACE      // trace constructors and destructors
#define NO_FD_TRACE       // trace file descriptors
#define RBLOGGER_ON
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

// tracing (as compared to loggin)
#ifdef NO_TRACE
    #define RBLOGTRACE( arg1, ...)
#else
    #define RBLOGTRACE(arg1, ...) \
        RBLogging::activeLogger.tracelog(\
            /*file */       (char*)__FILE__, \
            /*function:*/   (char*)__FUNCTION__, \
            /*line:*/       __LINE__, \
            /*arg1*/        arg1, \
            /*var args*/    ##__VA_ARGS__\
    )
#endif

// constructore/destructor trace
#ifdef NO_CTORTRACE
    #define RBLOGTORTRACE( arg_this )
#else
    #define RBLOGTORTRACE(arg_this) \
        RBLogging::activeLogger.torTraceLog(\
            /*file */       (char*)__FILE__, \
            /*function:*/   (char*)__FUNCTION__, \
            /*line:*/       __LINE__, \
            /*this*/        (void*)arg_this \
    )
#endif

#ifdef NO_FDTRACE
    #define RBLOGFDTRACE(arg_fd)
#else
    #define RBLOGFDTRACE(arg_fd) \
        RBLogging::activeLogger.fdTraceLog(\
            /*file */       (char*)__FILE__, \
            /*function:*/   (char*)__FUNCTION__, \
            /*line:*/       __LINE__, \
            /*this*/        arg_fd \
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

#define  LogError(arg1, ...)    ROBMACROLog(LOG_LEVEL_ERROR,   arg1, ##__VA_ARGS__)
#define  LogWarn(arg1, ...)     ROBMACROLog(LOG_LEVEL_WARN,    arg1, ##__VA_ARGS__)
#define  LogInfo(arg1, ...)     ROBMACROLog(LOG_LEVEL_INFO,    arg1, ##__VA_ARGS__)
#define  LogDebug(arg1, ...)    ROBMACROLog(LOG_LEVEL_DEBUG,   arg1, ##__VA_ARGS__)
#define  LogVerbose(arg1, ...)  ROBMACROLog(LOG_LEVEL_VERBOSE, arg1, ##__VA_ARGS__)

#define  LogTrace(arg1, ...)    RBLOGTRACE(arg1, ##__VA_ARGS__)
#define  LogTorTrace(...)       RBLOGTORTRACE(this)
#define  LogFDTrace(fd)         RBLOGFDTRACE(fd)

} // namespace RBLogging

#endif // header guard

#if ! defined(RBLOGGER_ENABLED)

    #define SET_LOGLEVEL(level)
    #define RBLOGGER_SETLEVEL(level) 

#else

    #define SET_LOGLEVEL(level) \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wunused-variable\"") \
    static RBLogging::LogLevelType rbLogLevel = level; \
    _Pragma("clang diagnostic pop")

    #define RBLOGGER_SETLEVEL(level) \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wunused-variable\"") \
    static  RBLogging::LogLevelType rbLogLevel = level; \
    _Pragma("clang diagnostic pop")

#endif
