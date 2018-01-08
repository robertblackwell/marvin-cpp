#ifndef tsc_pipeline_test_hpp
#define tsc_pipeline_test_hpp

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include "boost_stuff.hpp"
#include "rb_logger.hpp"
#include "buffer.hpp"
#include "client.hpp"
#include "bf_testcase.hpp"

namespace body_format {
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
            boost::asio::io_service&        m_io;
            Marvin::Http::MessageBaseSPtr   m_msg;
            Marvin::UriSPtr                 m_uri_sptr;
            ClientSPtr                      m_client;
            std::vector<Testcase>           m_testcase;
            int                             m_msg_index;
            std::string                     m_ch_uuid;
            std::string                     m_rh_uuid;
    };
}

#endif /* test_client_h */
