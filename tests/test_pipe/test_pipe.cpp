#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <iostream>
#include <string>
#include <vector>

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
struct ExtractResult {
	std::vector<std::string> lines;
	int						  next_posn;
	int						  last_eoln_posn;

};
///
/// Extract lines from a character buffer (signalled by '\n')
/// and leave last_eoln_posn as an index into buf[] of the last '\n'
/// if last_eoln == len - 1 then all the buffer contents were used
/// if not then the characters from last_eoln to len - 1 are the 
/// beginning of the next line, and the next read should start at &(buf[len])
///
ExtractResult extract_lines(char buf[], int buf_start_posn, int len)
{

}

class LineReader
{
	public:

	LineReader(bp::async_pipe& pipe ): m_pipe(pipe)
	{
		m_char_buf_start = 0;
	}
	void read_line(std::function<void(std::string line)> line_cb)
	{
		m_line_cb = line_cb;
		p_start_read();
	}

	private:

	void p_read_next()
	{
		p_start_read();
	}
	void p_start_read()
	{
		try {
			m_pipe.async_read_some( asio::buffer(&(m_char_buf[m_char_buf_start]), 1),
				[this](const boost::system::error_code &ec, std::size_t size) {
				if (size != 0) {
					if (m_char_buf[m_char_buf_start] == '\n') {
						std::string tmp = std::string(&(m_char_buf[0]), m_char_buf_start);
						m_char_buf_start = 0;
						m_line_cb(tmp);
					} else {
						m_char_buf_start++;
						p_read_next();
					}
				}
				else {
					std::cerr << "error " << ec << std::endl;
				}
			});
		} catch (std::system_error &ec) {
			std::cout << "S: system error: " << ec.what() << ", error code: " << ec.code() << std::endl;
		}
	} 

	bp::async_pipe& 						m_pipe;
	char 									m_char_buf[256];
	int 									m_char_buf_start;
	std::string 							m_line;
	std::function<void(std::string line)>  	m_line_cb;

};
class CtlDriver
{
	public:
	
	CtlDriver(std::string pipe_path): m_io_ctx(1), m_pipe(m_io_ctx, pipe_path), m_reader(m_pipe)
	{
		read_and_process_a_line();
		m_io_ctx.run();
	}
	void another_one()
	{
		read_and_process_a_line();
	}
	void read_and_process_a_line()
	{
		m_reader.read_line([this](std::string line)
		{
			std::cout << "S: got string from pipe 1 : [" << line << "] size: " << line.size() << std::endl << std::flush;
			// .... process a line.
			another_one();
		});
	}

	asio::io_context 	m_io_ctx;
	std::string 		m_pipe_path;
	bp::async_pipe 		m_pipe;
	LineReader 				m_reader;

};
int main(int argc, char *argv[])
{
	std::string pipe_path = "/home/robert/Projects/marvin++/.marvin/marvin_ctl";
	if (boost::filesystem::exists(pipe_path)) {
		boost::filesystem::remove(pipe_path);
	}
	CtlDriver driver(pipe_path);

}
