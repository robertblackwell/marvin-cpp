//
//  forwarding_handler.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/25/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef marvin_ssl_forwarding_handler_hpp
#define marvin_ssl_forwarding_handler_hpp

#include <cstdio>
#include <iostream>
#include <sstream>
#include <regex>
#include <marvin/server/request_handler_base.hpp>
#include <marvin/external_src/rb_logger/rb_logger.hpp>
#include <marvin/external_src/uri-parser/UriParser.hpp>
#include <marvin/client/client.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/http/headers_v2.hpp>
#include <marvin/connection/tunnel_handler.hpp>
#include <marvin/collector/collector_interface.hpp>

using namespace Marvin;
using namespace Marvin::Http;

enum class ConnectAction;
class SSLForwardingHandler;
using SSLForwardingHandlerSPtr = std::shared_ptr<SSLForwardingHandler>;
using SSLForwardingHandlerUPtr = std::unique_ptr<SSLForwardingHandler>;

std::string traceSSLForwardingHandler(SSLForwardingHandlerUPtr fh_ptr);
std::string traceSSLForwardingHandler(SSLForwardingHandlerSPtr fh_ptr);
std::string traceSSLForwardingHandler(SSLForwardingHandler* fh_ptr);

class SSLForwardingHandler
{
    public:
        // these are configuration settings
        static void configSet_HttpsHosts(std::vector<std::regex> re);
        static void configSet_HttpsPorts(std::vector<int> ports);
    
        SSLForwardingHandler(boost::asio::io_service& io, ICollector* collector);
        ~SSLForwardingHandler();

        void handleSSLRequest(
            ServerContext&              server_context,
            MessageReaderSPtr           request,
            MessageWriterSPtr           responseWriter,
            ConnectionSPtr              clientConnectionPtr,
            HandlerDoneCallbackType     done
        );

    private:
    
        // methods that are used in handleRequest
        void p_round_trip_upstream(
            MessageReaderSPtr req,
            std::function<void(Marvin::ErrorType& err, MessageBaseSPtr downstreamReplyMsg)> upstreamCb
        );
        void p_handle_upstream_response_received(Marvin::ErrorType& err);
        void p_make_downstream_response();
        void p_make_downstream_error_response(Marvin::ErrorType& err);
        void p_handle_upgrade();
        void p_on_complete(Marvin::ErrorType& err);
    
        // methods that are used in handleConnect
        void p_ssl_handshake_upstream(std::function<void(const boost::system::error_code& err)> cb);
        void p_on_handshake_complete(const boost::system::error_code& err);

        void p_ssl_send_OK_downstream(std::function<void(Marvin::ErrorType& err)> cb);
        void p_on_send_OK_complete(Marvin::ErrorType& err);

        void p_ssl_become_secure_downstream(std::function<void(Marvin::ErrorType& err)> cb);
        void p_on_become_secure_downstream_complete(Marvin::ErrorType& err);

        void p_ssl_read_request_from_downstream(std::function<void(Marvin::ErrorType& err)> cb);
        void p_on_read_request_downstream_complete(Marvin::ErrorType& err);

        void p_ssl_roundtrip_upstream(std::function<void(Marvin::ErrorType& err)> cb);
        void p_on_rountrip_complete(Marvin::ErrorType& err);



        // utility methods
        void p_response403Forbidden(MessageWriter& writer);
        void p_response200OKConnected(MessageWriter& writer);
        void p_response502Badgateway(MessageWriter& writer);

        boost::asio::io_service&    m_io;
        /// @brief Only used by the handleConnect method
        ConnectionSPtr              m_connSsptr;
        /// reader of the initial request from downstream - passed in to our handleRequest method
        MessageReaderSPtr           m_request_reader_sptr;
        /// writer of the final response to down stream - passed in to our handler request method
        MessageBaseSPtr             m_response_sptr;
        MessageWriterSPtr           m_response_writer_sptr;
        /// message object to hold final downstream response
        MessageBaseSPtr             m_downstream_msg_sptr;
        /// done callback - passed in to our handleRequest method
        HandlerDoneCallbackType     m_done_callback;
        Marvin::BufferChainSPtr     m_response_body_sptr;

        /// Client instance to handle upstream round trip
        ClientUPtr                  m_upstream_client_uptr;
        MessageBaseSPtr             m_upstream_request_msg_sptr;

    
        /// this will collect summaries of the req and resp
        std::string                 m_scheme;
        std::string                 m_host;
        int                         m_port;
        ICollector*                 m_collector_ptr;
    
        /// used for handleConnect - tunnel
        Marvin::MBufferUPtr         m_initial_response_buf;
        ConnectionSPtr              m_downstream_connection;
        ConnectionSPtr              m_upstream_connection; 
    
        /// regexs to define hosts that require mitm not tunnel
        std::function<void(std::string s, std::string h, MessageReaderSPtr req, MessageBaseSPtr resp)>       m_collect_function;

};
#endif /* forwarding_handler_hpp */
