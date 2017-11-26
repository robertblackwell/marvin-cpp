/**
* This file tests mutiple simultanious requests to by different client objects
* to ensure isloation between instances
*/


/**
* Perform a single get request
*/
std::shared_ptr<Request> do_get_request(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Request> req = std::shared_ptr<Request>(new Request(io));

#ifdef VERBOSE
    std::cout << "Create request 1 : " << std::hex << req.get() << std::endl;
#endif

    req->setMethod(HttpMethod::GET);
    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);

    std::string b("");
    req->setContent(b);
    req->go([ req, code](Marvin::ErrorType& ec){

        MessageReader& resp = req->getResponse();

#ifdef VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
        std::cout << "request " << std::hex << &resp << std::endl;
        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
        std::cout << "request " << resp.getBody() << std::endl;
        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
        std::string t = code+code+code+code+code+code+code+code+code + code;
        
        //
        // Here is where we check the result
        //
        bool x = (t == resp.getBody());
        std::cout << "test for :" << code;
        if( x )
        {
            std::cout << " OK";
        }
        else
        {
            std::cout << " FAIL";
        }
        std::cout << std::endl;
        LogDebug("");
    });
#ifdef VERBOSE
    std::cout << "exit use: " << req.use_count() << std::endl;
#endif
    return req;
}

void testcase_multiple()
{
    boost::asio::io_service io_service;
    {
        std::vector<std::shared_ptr<Request>> rt;
        rt.push_back(do_get_request("1", io_service));
        rt.push_back(do_get_request("A", io_service));
        rt.push_back(do_get_request("B", io_service));
        rt.push_back(do_get_request("C", io_service));
        rt.push_back(do_get_request("D", io_service));
        rt.push_back(do_get_request("E", io_service));
        rt.push_back(do_get_request("F", io_service));
        rt.push_back(do_get_request("G", io_service));
        io_service.run();
        rt.clear();
    }
    std::cout << "after io_service.run() " << std::endl;
}

