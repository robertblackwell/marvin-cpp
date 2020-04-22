#include <marvin/error_handler/exception.hpp>
namespace Marvin {

void errorHandler(std::string func, std::string file, int line, std::string msg) 
{
	std::string message(msg);
	std::stringstream messageStream;
	messageStream << "Error in function: " << func << " file: " << file << " line: " << line << " Message: " << message;
	throw ::Marvin::Exception(messageStream.str());
}


Exception::Exception(std::string message) : marvin_message(message){}
const char* Exception::what() const noexcept {return marvin_message.c_str();} 
} // namespace Marvin