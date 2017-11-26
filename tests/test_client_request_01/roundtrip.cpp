
std::shared_ptr<Client> do_client_connect(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn/utests/echo/test.php"));
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    client->asyncConnect([client, msg, code](Marvin::ErrorType& ec){
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
#endif
    });

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}

std::shared_ptr<Client> one_roundtrip(std::string code, boost::asio::io_service& io)
{
    std::shared_ptr<Client> client = std::shared_ptr<Client>(new Client(io, "http://whiteacorn.com/posts/rtw" ));
    
    std::shared_ptr<MessageBase> msg = std::shared_ptr<MessageBase>(new MessageBase());
    
//    msg->setIsRequest(true);
//    msg->setHttpVersMajor(1);
//    msg->setHttpVersMinor(1);
//    msg->setUri("/posts/rtw");
    msg->setMethod(HttpMethod::GET);
    
//    msg->setHeader(HttpHeader::Name::Host, "whiteacorn.com");
//    msg->setHeader(HttpHeader::Name::Connection, "Close");
//    msg->setHeader(HttpHeader::Name::ContentLength, "0");

    
    std::function<void(Marvin::ErrorType& er, MessageReaderV2SPtr rdr)> f = [client, msg, code](Marvin::ErrorType& ec, MessageReaderV2SPtr rdr) {
#if 1 //VERBOSE
        std::cout << "request " << "Error " << ec.value() << " " << ec.message() << std::endl;
        std::cout << "request " << std::hex << client.get() << std::endl;
//        std::cout << "request " << std::hex << &resp << std::endl;
//        std::cout << "request " << resp.statusCode() << " " << resp.status() << std::endl;
//        std::cout << "request " << resp.getBody() << std::endl;
//        std::cout << "request " << std::hex << req.get() << std::endl;
        MessageReaderV2SPtr b = client->getResponse();
        std::string bdy = b->getBody();
        std::cout << bdy << std::endl;
        
#endif
    };
    client->asyncWrite(msg, f);

//    req->setMethod(HttpMethod::GET);
//    req->setUrl("http://whiteacorn/utests/echo/test.php?code="+code);
//
//    std::string b("");
//    req->setContent(b);
//    req->go([ req, code](Marvin::ErrorType& ec){
    return client;

}


void testcase_roundtrip()
{
    boost::asio::io_service io_service;
    std::vector<std::shared_ptr<Client>> rt;
    rt.push_back(one_roundtrip("1", io_service));
#if 0
    rt.push_back(one_roundtrip("A", io_service));
    rt.push_back(one_roundtrip("B", io_service));
    rt.push_back(one_roundtrip("C", io_service));
    rt.push_back(one_roundtrip("D", io_service));
    rt.push_back(one_roundtrip("E", io_service));
#endif
    io_service.run();
    rt.clear();
    std::cout << "after io_service.run() " << std::endl;
}

