#include <marvin/error_handler/error_handler.hpp>

#include <sstream>
#include <exception>
namespace Marvin {
/**
*/
void errorHandler (std::string func, std::string file, int lineno, std::string msg)
{
    std::string message(msg);
    std::stringstream messageStream;
    messageStream <<  "Error in function: " << func << " file: " << file << " at lineNo: " << lineno << std::endl << "Message: [" << message << "]" ;
    throw Exception(messageStream.str());
}

Exception::Exception(std::string aMessage) : errMessage(aMessage){}
const char* Exception::what() const noexcept{ return errMessage.c_str(); }

} // namespace
