#ifndef tsc_pipeline_test_hpp
#define tsc_pipeline_test_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <marvin/include/boost_stuff.hpp>
#include <marvin/external_src/rb_logger.hpp>
#include <marvin/buffer/buffer.hpp>
#include <marvin/client/client.hpp>
#include "tp_testcase.hpp"

namespace tp {
    class PipelineTest;
    typedef std::shared_ptr<PipelineTest> PipelineTestSPtr;
    /**
    * Could also have been called keep-alive tester.
    */
    class PipelineTest
    {
        public:
            PipelineTest(boost::asio::io_service& io, std::vector<Testcase> testcase);
            void handler(Marvin::ErrorType& er, MessageReaderSPtr rdr);
            void exec();
        protected:
            boost::asio::io_service&    _io;
            MessageBaseSPtr             _msg;
            ClientSPtr                  _client;
            std::vector<Testcase>       _testcase;
            int                         _msg_index;
            std::string                 _ch_uuid;
            std::string                 _rh_uuid;
    };
}

#endif /* test_client_h */
