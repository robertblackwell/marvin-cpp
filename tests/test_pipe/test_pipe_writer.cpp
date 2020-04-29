#include <iostream>
#include <fstream>

#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/process/async_pipe.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
	namespace bp = boost::process;
	namespace asio = boost::asio;
	asio::io_context pipe_context;
	try {

		std::string pipe_path = "/home/robert/Projects/marvin++/.marvin/marvin_ctl";
		
		std::fstream f(pipe_path);

		f << "This is a message I hope will work\n" << std::endl << std::flush;
		return 1;
		bp::async_pipe pipe(pipe_context, pipe_path);
		asio::streambuf buf;
		std::ostream output(&buf);
		output << "This is a message from writer " << std::endl << std::flush;
		std::string s;
		// std::getline(std::cin, s);
		std::string output_s = "This is a message from writer ";
		asio::const_buffer b(output_s.c_str(), output_s.size());

		asio::async_write(pipe, b,
			[&buf](const boost::system::error_code &ec, std::size_t size) {
			if (size != 0) {
				std::cout << "C: sent string from pipe: \"" << &buf << "\"" << std::endl;
			}
			else {
				std::cerr << "error " << ec << std::endl;
			}
		}
		);
		pipe_context.run();
	} catch (std::system_error &ec) {
		std::cout << "C: system error: " << ec.what() << ", error code: " << ec.code() << std::endl;
	} 
}