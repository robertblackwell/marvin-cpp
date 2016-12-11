
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "handler_interface.hpp"

class MyHandler : public RequestHandlerInterface
{
public:
    void handle_request(const request& req, reply& rep, HandlerDoneCallbackType done){
        std::cout << "got to a new handler " << std::endl;
        rep = reply::stock_reply(reply::ok_just_joking);
        rep.status = reply::ok;
        rep.content = "<html><body>hello world</body></html>";
        rep.headers.resize(1);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = std::to_string(rep.content.size());
        done(true);
    }
};


int main(int argc, char* argv[])
{
    try
    {
        MyHandler handler;
        Server server(handler);
        server.listen();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
