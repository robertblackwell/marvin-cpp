//
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <bitset>
#include "rb_logger.hpp"

bool RBLogger::logger_enabled = true;
RBLogger::LogLevelType RBLogger::Logger::allEnabled = 
    RBLogger::LogLevelVerbose | RBLogger::LogLevelFDTrace | RBLogger::LogLevelTrace | RBLogger::LogLevelCTorTrace;

RBLogger::LogLevelType RBLogger::Logger::globalThreshold = RBLogger::Logger::allEnabled; 

RBLogger::Logger RBLogger::Logger::activeLogger{};


std::ostringstream& RBLogger::preamble(
    std::ostringstream& os,
    std::string filename,
    long pid,
    long tid,
    std::string function_name,
    long linenumber
){
    os 
        #ifdef RBLOG_FILENAME
        << filename.c_str() 
        #endif
        #ifdef RBLOG_PIDTID
        << "[" 
        <<pid 
        << ":" 
        << tid 
        <<"]" 
        #endif
        #ifdef RBLOG_FUNCTION_NAME
        << "::"
        << function_name 
        #endif
        #ifdef RBLOG_LINE_NUMBER
        << "["<< linenumber <<"]"
        << ":" 
        #endif
        << "";
    return os;
}

void RBLogger::setEnabled(bool on_off)
{
    logger_enabled = on_off;
}
void RBLogger::enableForLevel(LogLevelType level)
{
    RBLogger::Logger::globalThreshold  = level;
    logger_enabled = true;
}

std::string RBLogger::Logger::p_className(std::string& func_name){
    
    return "";
}

RBLogger::Logger::Logger(std::ostream& os) : m_outStream(os)
{
    RBLogger::logger_enabled = true;
}


std::string RBLogger::LogLevelText(RBLogger::LogLevelType level)
{
    static std::string tab[] = {
        "",
        "ERR",
        "WRN",
        "INF",
        "DBG",
        "VRB",
    };
    static std::string other_tab[] = {
        "BAD1",
        "TRC","TOR",
        "BAD3","FD "
    };
    long adjusted_level;
    long level_long = level;
    if (level > 4) {
        std::bitset<8> blevel(level);
        adjusted_level = level >> 3;
        std::bitset<8> badjusted_level(adjusted_level);
        // std::cout << "LogLevelText level: " << blevel << " adjusted_level : " << badjusted_level << std::endl;
        assert(adjusted_level < 6);
        return tab[adjusted_level];
    } else {
        adjusted_level = (level & 0b00000111);
        return other_tab[adjusted_level];
    }
    return tab[(int)level];
}

void RBLogger::Logger::logWithFormat(
     LogLevelType           level,
     LogLevelType           threshold,
      const char*    file_name,
     const char*    func_name,
     int            line_number,
     char*          format,
     ...)
{
    using namespace boost::filesystem;
    std::ostringstream os;
    if( levelIsActive(level, threshold) ){
        std::lock_guard<std::mutex> lg(_loggerMutex);
        os << RBLogger::LogLevelText(level) << "|";
        path tmp2 = path(file_name);
        path filename_tmp3 = tmp2.filename();
        path tmp4 = filename_tmp3.stem();
        auto tmp5 = tmp4.string();
        auto pid = ::getpid();
        auto tid = pthread_self();

        #ifndef RBLOG_USE_PREAMBLE
        os 
            << tmp3.c_str() 
            << ":" 
            << "[" 
            <<pid 
            << ":" 
            << tid 
            <<"]" 
            << func_name 
            << "["<< line_number <<"]"
            << ":" ;
        #else
        preamble(os, filename_tmp3.string(), pid, tid, func_name, line_number);
        #endif

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
void RBLogger::Logger::torTraceLog(
    LogLevelType           level,
    LogLevelType           threshold,
    const char* file_name,
    const char* func_name,
    int line_number,
    void* this_arg)
{
    if( levelIsActive(level, threshold) ){
        std::ostringstream os;
        std::lock_guard<std::mutex> lg(_loggerMutex);
        
        os << "CTR" <<"|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto filename_tmp3 = tmp2.filename();
        auto tmp4 = filename_tmp3.stem();
        auto pid = ::getpid();
        auto tid = pthread_self();

        #ifndef RBLOG_USE_PREAMBLE
        os 
            << filename_tmp3.c_str() 
            << ":" 
            << "[" 
            <<pid 
            << ":" 
            << tid 
            <<"]" 
            << func_name 
            << "["<< line_number <<"]"
            << ":" ;
        #else
        preamble(os, filename_tmp3.string(), pid, tid, func_name, line_number);
        #endif

        os << std::hex << (long)this_arg << std::dec << std::endl;;
        //
        // Only use the stream in the last step and this way we can send the log record somewhere else
        // easily
        //
        write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    }
}
void RBLogger::Logger::fdTraceLog(
    LogLevelType level,
    LogLevelType threshold,
    const char* file_name,
    const char* func_name,
    int line_number,
    long fd_arg)
{
    if( levelIsActive(level, threshold) ){
        std::ostringstream os;
        std::lock_guard<std::mutex> lg(_loggerMutex);
        
        os << "FD " <<"|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto filename_tmp3 = tmp2.filename();
        auto tmp4 = filename_tmp3.stem();
        auto pid = ::getpid();
        auto tid = pthread_self();

        #ifndef RBLOG_USE_PREAMBLE
        os 
            << filename_tmp3.c_str() 
            << ":" 
            << "[" 
            <<pid 
            << ":" 
            << tid 
            <<"]" 
            << func_name 
            << "["<< line_number <<"]"
            << ":" ;
        #else
        preamble(os, filename_tmp3.string(), pid, tid, func_name, line_number);
        #endif
        os << " fd:" << fd_arg << std::endl;;
        //
        // Only use the stream in the last step and this way we can send the log record somewhere else
        // easily
        //
        write(STDERR_FILENO, os.str().c_str(), strlen(os.str().c_str()) );
    }
}


bool RBLogger::Logger::enabled()
{
    /// this function is only used for Trace functions
    /// we want these active with DEBUG levels
    LogLevelType lvl = LOG_LEVEL_DEBUG;
    LogLevelType tmp = globalThreshold;
    return ( ((int)lvl <= (int)tmp) && RBLogger::logger_enabled );
}
bool testLevelForActive(long level, long threshold )
{
	long result;
	long threshold_bits;
	std::bitset<8> blevel(level);
	std::bitset<8> bthreshold(threshold);
	// std::cout << "testLevels entry level: " << blevel << " threshold: " << bthreshold << std::endl;
	if (level <= 4) {
		threshold_bits = (threshold & 0x07);
		std::bitset<8> bthreshold_bits(threshold);
		result = (level & threshold_bits);
		std::bitset<8> bresult(result);
		// std::cout << "testLevels level =< 4 threshold_bits: " << bthreshold_bits << " result: " << bresult << std::endl;
		return result;
	} else {
		return (level <= threshold);
	}
}

bool RBLogger::Logger::levelIsActive(LogLevelType lvl, LogLevelType threshold)
{
    if (! RBLogger::logger_enabled)
        return false;
    if (testLevelForActive(lvl, threshold)) {
        if (testLevelForActive(lvl, globalThreshold)) {
            return true;
        } else {
            return false;
        }
    }
    return false;
    /// use the lowest threshold - local or global
    LogLevelType tmp = (threshold <= globalThreshold) ? threshold : globalThreshold;
    return ( ((int)lvl <= (int)tmp) && RBLogger::logger_enabled );
//    return ( ((int)lvl <= (int)threshold) && RBLogger::logger_enabled );
}

void RBLogger::Logger::myprint(std::ostringstream& os)
{
//        write(STDERR_FILENO, "\n", 2);
    os << std::endl;
}
