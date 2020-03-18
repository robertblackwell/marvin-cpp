//
//
//
//
//
//
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include "rb_logger.hpp"

bool RBLogging::logger_enabled = true;
RBLogging::LogLevelType RBLogging::globalThreshold = LOG_LEVEL_MAX; // enabled everything
RBLogging::FilePathListType RBLogging::activeFileStems;

void RBLogging::setEnabled(bool on_off)
{
    logger_enabled = on_off;
}
void RBLogging::enableForLevel(LogLevelType level)
{
    RBLogging::globalThreshold  = level;
    logger_enabled = true;
}

std::string RBLogging::Logger::p_className(std::string& func_name){
    
    return "";
}

RBLogging::Logger::Logger(std::ostream& os) : m_outStream(os)
{
    RBLogging::logger_enabled = true;
}


std::string RBLogging::LogLevelText(RBLogging::LogLevelType level){
    static std::string tab[] = {
        "",
        "ERROR",
        "WARN",
        "TRACE",
        "MTRAC",
        "INFO",
        "DEBG",
        "VERB",
    };
    return tab[(int)level];
}
bool RBLogging::Logger::p_fileStemIsActive(RBLogging::FilePathType file_path)
{
    auto stem = file_path.stem();
//    auto f = RBLogging::activeFileStems.find(stem);
//    auto xx = *f;
    if (RBLogging::activeFileStems.find(stem) != RBLogging::activeFileStems.end()) {
        return true;
    }
    return false;
}



void RBLogging::Logger::logWithFormat(
                             LogLevelType           level,
                             LogLevelType           threshold,
                              const char*    file_name,
                             const char*    func_name,
                             int            line_number,
                             char*          format,
                             ...)
{
    std::ostringstream os;
    if( levelIsActive(level, threshold) ){
        std::lock_guard<std::mutex> lg(_loggerMutex);
        os << RBLogging::LogLevelText(level) << "|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto tmp3 = tmp2.filename();
        auto tmp4 = tmp3.stem();
        auto tmp5 = tmp4.string();
        auto pid = ::getpid();
        auto tid = pthread_self();

        os << tmp3.c_str() ;
        os << ":" << "[" <<pid << ":" << tid <<"]" << func_name << "["<< line_number <<"]:" ;
        va_list argptr;
        va_start(argptr,format);
        char* bufptr;
        vasprintf(&bufptr, format, argptr);
        va_end(argptr);
        std::string outStr = os.str() + std::string(bufptr, strlen(bufptr));
        const char* outCharStar = outStr.c_str();
        size_t len = strlen(outCharStar);
        write(STDERR_FILENO, (void*)outCharStar, len);
    }
}
void RBLogging::Logger::torTraceLog(
              const char* file_name,
              const char* func_name,
              int line_number,
              void* this_arg)
{
    if( enabled())
    {
        std::ostringstream os;
        std::lock_guard<std::mutex> lg(_loggerMutex);
        
        os << "CTOR" <<"|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto tmp3 = tmp2.filename();
        auto tmp4 = tmp3.stem();
        auto pid = ::getpid();
        auto tid = pthread_self();


        os <<  tmp3.c_str() << "[" << pid << ":" << tid << "]";
        os << "::"<< func_name << "[" << line_number << "]: " << std::hex << (long)this_arg << std::dec << std::endl;;
        //
        // Only use the stream in the last step and this way we can send the log record somewhere else
        // easily
        //
        write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    }
}
void RBLogging::Logger::fdTraceLog(
              const char* file_name,
              const char* func_name,
              int line_number,
              long fd_arg)
{
    if (enabled()) {
        std::ostringstream os;
        std::lock_guard<std::mutex> lg(_loggerMutex);
        
        os << "FD" <<"|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto tmp3 = tmp2.filename();
        auto tmp4 = tmp3.stem();
        auto pid = ::getpid();
        auto tid = pthread_self();


        os <<  tmp3.c_str() << "[" << pid << ":" << tid << "]";
        os << "::"<< func_name << "[" << line_number << "] fd:" << fd_arg << std::endl;;
        //
        // Only use the stream in the last step and this way we can send the log record somewhere else
        // easily
        //
        write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    }
}
bool RBLogging::Logger::enabled()
{
    /// this function is only used for Trace functions
    /// we want these active with DEBUG levels
    LogLevelType lvl = LOG_LEVEL_DEBUG;
    LogLevelType tmp = globalThreshold;
    return ( ((int)lvl <= (int)tmp) && RBLogging::logger_enabled );
}
bool RBLogging::Logger::levelIsActive(LogLevelType lvl, LogLevelType threshold)
{
    /// use the lowest threshold - local or global
    LogLevelType tmp = (threshold <= globalThreshold) ? threshold : globalThreshold;
    return ( ((int)lvl <= (int)tmp) && RBLogging::logger_enabled );
//    return ( ((int)lvl <= (int)threshold) && RBLogging::logger_enabled );
}
void RBLogging::setActiveFileStems(RBLogging::FilePathListType stems)
{
    RBLogging::activeFileStems = stems;
}
void RBLogging::addTraceFile(std::string filepath_string)
{
    auto pth = boost::filesystem::path(filepath_string);
    auto stm = pth.stem();
    RBLogging::activeFileStems.insert(stm);
}
void RBLogging::addTraceFile(const char* stem_string)
{
    const std::string s(stem_string);
    RBLogging::addTraceFile(s);
}
void RBLogging::Logger::myprint(std::ostringstream& os)
{
//        write(STDERR_FILENO, "\n", 2);
    os << std::endl;
}
#ifdef LOGGER_SINGLE
RBLogging::Logger activeLogger{};
#else
static RBLogging::Logger activeLogger{};
#endif
