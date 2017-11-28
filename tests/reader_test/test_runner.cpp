/**
* Class TestRunner - knows how to run a different test scenario on a single testcase
*/
class TestRunner
{
    
public:
    MyMessageReader*              rdr_;
    MockReadSocket              conn_;
    boost::asio::io_service&    io_;
    std::string                 body;
    std::ostringstream          bodyStream;
    Testcases                   tcObjs;
    Testcase                    tcObj;
    int                         _tcIndex;


    /**
    * Constructor - tcIndex is an index into the set of testcases
    * that the class TestCases knows about
    */
    TestRunner(boost::asio::io_service& io, int tcIndex)
        : io_(io),
        _tcIndex(tcIndex),
        conn_(MockReadSocket(io, tcIndex)),
        tcObjs(Testcases()),
        tcObj(tcObjs.get_case(tcIndex))
    {
        LogDebug("");
        rdr_ = new MyMessageReader(&conn_, io_);
        body = std::string("");
        bodyStream.str(body);
    }
    ~TestRunner()
    {
        LogDebug("");
        delete rdr_;
    }
    void onMessage(Marvin::ErrorType& er)
    {
        LogDebug("");
        assert(!er);
        assert(rdr_->statusCode() == tcObj.result_status_code());
        auto h1 = tcObj.result_headers();
        auto h2 = rdr_->getHeaders();
        bool hh = (h1 == h2);
        assert(tcObj.result_headers() == rdr_->getHeaders());
        auto b1 = tcObj.result_body();
        auto b2 = rdr_->getBody();
        assert(b1 == b2);
        auto desc = tcObj.getDescription();
        std::cout << "TestRunner::readMessage Success for testcase " << tcObj.getDescription() <<std::endl;
    }
    void onBody(Marvin::ErrorType& er, FBuffer* fBufPtr)
    {
        LogDebug(" entry");
        // are we done - if not hang another read
        auto bh = std::bind(&TestRunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
        bool done = (er == Marvin::make_error_eom());
        
        if( done )
        {
            // body comes in different way for this type of read
            bodyStream << *fBufPtr;
            delete fBufPtr;
            std::string msgStr = rdr_->MessageBase::str() + bodyStream.str();
            body = bodyStream.str();
            auto body2 = rdr_->getBody();
            std::string expectedBody = tcObj.result_body();
            
            bool vb = tcObj.verify_body(body);
            assert(vb);
            assert(er == Marvin::make_error_eom());
            auto desc = tcObj.getDescription();
            std::cout << "TestRunner::run_StreamingBodyRead Success testcase " << tcObj.getDescription() <<std::endl;

        }else{
            // do something with fBuf
            //
            // lets accumulate the FBuffer into a body
            //
            std::string xx = bodyStream.str();
            bodyStream << *fBufPtr;
            delete fBufPtr;
            rdr_->readBody(bh);
        }
        LogDebug("exit");
        
    }
    void onHeaders(Marvin::ErrorType& er){
        LogDebug("entry");
        Marvin::ErrorType expected_err = tcObj.result_onheaders_err();
        std::string ers = er.message();
        assert(er == expected_err);
        assert(rdr_->statusCode() == tcObj.result_status_code());
        auto h1 = tcObj.result_headers();
        auto h2 = rdr_->getHeaders();
        bool hhh = tcObj.verify_headers(h2);
        assert(hhh);
        auto bh = std::bind(&TestRunner::onBody, this, std::placeholders::_1, std::placeholders::_2);
        rdr_->readBody(bh);
        LogDebug("exit");
    }
    /**
    * runs a test that reads reads a full message
    */
    void run_FullMessageRead()
    {
        LogDebug("getting started");
        auto h = std::bind(&TestRunner::onMessage, this, std::placeholders::_1);
        rdr_->readMessage(h);
    }
    /**
    * runs a test that reads the headers only
    */
    void run_StreamingBodyRead()
    {
        LogDebug("getting started");
        auto h = std::bind(&TestRunner::onHeaders, this, std::placeholders::_1);
        rdr_->readHeaders(h);

    }
};
