//
//
//
//
//
//
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "RBLogger.hpp"

std::string RBLogging::Logger::className(std::string& func_name){
    
    return "";
}


std::string RBLogging::LogLevelText(RBLogging::LogLevelType level){
    std::string tab[] = {
        "",
        "ERROR",
        "WARN",
        "INFO",
        "DEBG",
        "VERB"
    };
    return tab[(int)level];
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
        os << RBLogging::LogLevelText(level) << "|";
        auto tmp2 = boost::filesystem::path(file_name);
        auto tmp3 = tmp2.filename();
        auto tmp4 = tmp3.stem();
        auto tmp5 = tmp4.string();
        auto tmp6 = tmp4.c_str();
        os << tmp3.c_str() ;
        os << ":" << func_name << "["<< line_number <<"]:" ;
        va_list argptr;
        va_start(argptr,format);
        char* bufptr;
        vasprintf(&bufptr, format, argptr);
        va_end(argptr);
        __outStream << os.str() << bufptr << std::endl;
    }
}

static RBLogging::Logger activeLogger{};
