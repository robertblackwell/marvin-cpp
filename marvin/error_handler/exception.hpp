#ifndef guard_marvin_exception_hpp
#define guard_marvin_exception_hpp

namespace Marvin {
void errorHandler(std::string func, std::string file, int line, std::string msg);
class Exception : public std::exception 
{
public:
	Exception(std::string message);
	const char* what() const noexcept;
protected:
	std::string marvin_message;
};

} // namespace Marvin

#endif