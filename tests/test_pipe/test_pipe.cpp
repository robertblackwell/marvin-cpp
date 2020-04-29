#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <iostream>

namespace bp = boost::process;
namespace asio = boost::asio;
#if 0
class PipeReader
{
	public:

	asio::io_context pipe_context;
	asio::streambuf buf;

	PipeReader(std::string pipe_path): pipe_context(1)
	{
		if (boost::filesystem::exists(pipe_path)) {
			boost::filesystem::remove(pipe_path);
		}
		bp::async_pipe pipe(pipe_context, pipe_path);
		start_read();
		pipe_context.run();

	}
	void start_read() {
		asio::streambuf buf;
		
		asio::async_read_some(pipe, buf, 
			[this, &buf](const boost::system::error_code &ec, std::size_t size) {
			if (size != 0) {
				std::cout << "S: got string from pipe: \"" << &buf << "\"" << std::endl;
				std::cout << "S: got string from pipe: \"" << &buf << "\"" << std::endl;
				handle_read();
			}
			else {
				std::cerr << "error " << ec << std::endl;
			}
		});

	}
	void handle_read() {
		std::cout << "S: got string from pipe: \"" << &buf << "\"" << std::endl;
		std::cout << "S: got string from pipe: \"" << &buf << "\"" << std::endl;
		start_read();
	}
};
#endif
class Reader
{
	public:
	asio::io_context pipe_context;
	std::string pipe_path;
	bp::async_pipe pipe;
	asio::streambuf m_buf;
	Reader(): pipe_context(1), pipe_path("/home/robert/Projects/marvin++/.marvin/marvin_ctl"), pipe(pipe_context, pipe_path)
	{

		start_read();
	}
	void read_next()
	{

		start_read();
	}
	void start_read()
	{
		try {
			asio::streambuf buf;
			asio::async_read_until(pipe, m_buf, "\n",
				[this, &buf](const boost::system::error_code &ec, std::size_t size) {
				if (size != 0) {
					m_buf.commit(size);
					boost::asio::streambuf::const_buffers_type bufs = m_buf.data();
					std::string line(
						boost::asio::buffers_begin(bufs),
						boost::asio::buffers_begin(bufs) + size);
					std::cout << "S: got string from pipe 1 : [" << line << "]" << std::endl << std::flush;
					read_next();
				}
				else {
					std::cerr << "error " << ec << std::endl;
				}
			}
			);
			pipe_context.run();
		} catch (std::system_error &ec) {
			std::cout << "S: system error: " << ec.what() << ", error code: " << ec.code() << std::endl;
		}
	} 
};
int main(int argc, char *argv[])
{
	std::string pipe_path = "/home/robert/Projects/marvin++/.marvin/marvin_ctl";
	if (boost::filesystem::exists(pipe_path)) {
		boost::filesystem::remove(pipe_path);
	}

	Reader r;
}
