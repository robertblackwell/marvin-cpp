#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

struct membuf
    : std::streambuf 
{
        membuf(char* base, std::size_t size) {
        this->setp(base, base + size);
        this->setg(base, base, base + size);
    }
    std::size_t written() const { return this->pptr() - this->pbase(); }
    std::size_t read() const    { return this->gptr() - this->eback(); }
};

int main() {
    // obtain a buffer starting at base with size size
	char base[100];
	boost::asio::streambuf sbuf;
    std::ostream out(&sbuf);
	auto x0 = sbuf.prepare(8);
	char* c0 = (char*)x0.data();
	strncpy((char*)x0.data(), "abcdefgh", 8);
	sbuf.commit(6);
    // out.write("ABCDEF", 6); // write three digits and three null chars
    // out.write("ABCDEF", 6); // write three digits and three null chars
	auto x1 = sbuf.prepare(7);
	strncpy((char*)x1.data(), "123456789", 7);
	sbuf.commit(5);
	sbuf.consume(13);
	auto x2 = sbuf.prepare(1);
	char* c1 = (char*)x2.data();
	std::cout << "" << std::endl;
}