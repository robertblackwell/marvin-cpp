#ifndef tsc_pipeline_test_hpp
#define tsc_pipeline_test_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "rb_logger.hpp"
#include "bufferV2.hpp"
#include "client.hpp"
#include "tsc_testcase.hpp"

class PipelineTest;
typedef std::shared_ptr<PipelineTest> PipelineTestSPtr;

class PipelineTest
{
    public:
        PipelineTest(boost::asio::io_service& io, std::vector<EchoTestcase>& testcase);
        void handler(Marvin::ErrorType& er, MessageReaderV2SPtr rdr);
        void exec();
    protected:
        boost::asio::io_service&    _io;
        MessageBaseSPtr             _msg;
        ClientSPtr                  _client;
        std::vector<EchoTestcase>&  _testcase;
        int                         _msg_index;
        std::string                 _uuid;
};


#endif /* test_client_h */
