
#ifndef marvin_mitm_app_hpp
#define marvin_mitm_app_hpp

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <regex>

#include <marvin/client/client.hpp>
#include <marvin/collector/collector_interface.hpp>
#include <marvin/connection/connection.hpp>
#include <marvin/connection/tunnel_handler.hpp>
#include<uri-parser/UriParser.hpp>
#include <marvin/http/header_fields.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/message/message_reader_v2.hpp>
#include <marvin/server_v3/request_handler_interface.hpp>
#include <marvin/collector/collector_interface.hpp>
#include <marvin/collector/capture_filter.hpp>
#include <marvin/server_v3/adapter.hpp>
namespace Marvin {

class MitmHttps;
class MitmHttp;
class MitmTunnel;

enum class ConnectAction;
class MitmApp;

typedef std::shared_ptr<MitmApp> MitmAppSPtr;
typedef std::unique_ptr<MitmApp> MitmAppUPtr;

/**
* \defgroup mitmapp MitmProxy
* \brief This is the group of classes that combine implement the MitmProxy application.
*/
/*
* \ingroup mitmapp
* This class is the primary interface between the TcpServer and its related objects and
* the man-in-the-middle http/https proxy server application.
*
* In order to interface to the TcpServer complex of objects this class is required to 
* implement the RequestHandlerInterface.
*
* There is one instance of this class for each open client connection being served.
* 
* In order to satisfies the clients request instances of this class (or its subordinates)
* will open a connection to the intended server and pass on the clinets requests,
* typically with some modifications to the headers of the request. These modifications
* are typical for a proxy and the details can be found in the relevant standard for proxies.
*
* A given request may be handled in one of 3 ways, __http__,  __https tunnel__, __https mitm__.
* Each different treatment is handled by a separate class that is closely coupled to
* the instance of MitmApp. These classes are  :
*   -   MitmHttp
*   -   MitmTunnel
*   -   MitmHttps
*
* the close coupling is implemented by making the subordinate classes friends of MitmApp. 
*/
class MitmApp : public Marvin::RequestHandlerInterface
{
    public:
        // these are configuration settings
        static void configSet_HttpsHosts(std::vector<std::string> re_string);
        static void configSet_HttpsHosts(std::vector<std::regex> re);
        static void configSet_HttpsPorts(std::vector<int> ports);
        static std::vector<std::string>& configGet_HttpsHosts(); 
        static std::vector<int>& configGet_HttpsPorts(); 
    
        MitmApp(boost::asio::io_service& io, ICollectorSPtr collector_sptr, CaptureFilter::SPtr filter_sptr);
        ~MitmApp();
        
        void handle(
            Marvin::ServerContext&            server_context,
            ISocketSPtr                       connPtr,
            Marvin::HandlerDoneCallbackType   done
        );

        void handle_request();

        friend class MitmHttps;
        friend class MitmHttp;
        friend MitmTunnel;
    
    private:
    
        std::unique_ptr<MitmHttps>  m_mitm_secure_uptr;
        std::unique_ptr<MitmHttp>   m_mitm_http_uptr;
        std::unique_ptr<MitmTunnel> m_mitm_tunnel_uptr;

        void p_on_completed();
    
        // methods that are used in handle_request
        void p_read_first_message();
        // void p_read_another_message();
        void p_on_first_message();

        // methods that are used in handleConnect
        ConnectAction p_determine_connection_action(std::string host, int port);
        // void p_initiate_tunnel();
        // void p_initiate_https_upstream_roundtrip();
        // void p_initiate_http_upstream_roundtrip();

        // void p_roundtrip_upstream(
        //     MessageReaderSPtr req,
        //     std::function<void(MessageBaseSPtr downstreamReplyMsg)> upstreamCb
        // );

        // called to signal that a tunnel has completed
        // so end the connection as keep-alive should have
        //been handled inside the tunnel
        void p_on_tunnel_completed();
        // called to signal that a request/response cycle has completed without error
        // only real job now is to determine if we are doing keep-alive and should do another read of
        // end the connection
        // void p_on_request_completed();
        // called to signal no more request will be processed.
        // give control back to the adapter or connection handler
        void p_connection_end();

        void p_on_downstream_read_error(Marvin::ErrorType& err);
        void p_on_downstream_write_error(Marvin::ErrorType& err);
        
        void p_on_upstream_error(Marvin::ErrorType& err);
        void p_on_upstream_roundtrip_error(Marvin::ErrorType& err);
        
        void p_on_tunnel_error(Marvin::ErrorType& err);


        void p_log_error(std::string label, Marvin::ErrorType err);

        // void p_handle_upstream_response_received(Marvin::ErrorType& err);
        // void p_make_downstream_response();
        // void p_make_downstream_error_response(Marvin::ErrorType& err);
        // void p_handle_upgrade();

        ConnectAction p_determine_action(std::string host, std::string port);
        
        // utility methods
        // void p_response403Forbidden(MessageWriter& writer);
        // void p_response200OKConnected(MessageWriter& writer);
        // void p_response502Badgateway(MessageWriter& writer);

        static std::vector<std::string>     s_https_host_strings;
        /// regexs to define hosts that require mitm not tunnel
        std::vector<std::regex>             m_https_hosts;
        /// list of port numbers that can be https mitm'd rather than tunneled
        std::vector<int>                    m_https_ports;

        // std::function<void(std::string s, std::string h, MessageReaderSPtr req, MessageBaseSPtr resp)> m_collect_function;

        boost::uuids::uuid                  m_uuid;
        std::string                         m_uuid_str;
        boost::asio::io_service&            m_io;
        ICollectorSPtr                      m_collector_sptr;
        ISocketSPtr                         m_socket_sptr;
        MessageWriterSPtr                   m_wrtr;
        MessageReaderV2::SPtr               m_rdr;
        MessageBase::SPtr                   m_request_msg_sptr;
        std::string                         m_body;
        Marvin::HandlerDoneCallbackType     m_done;
        ATimerSPtr                          m_timer_sptr;
        std::function<void()>               m_done_callback;
        std::string                         m_scheme;
        std::string                         m_host;
        std::string                         m_port;
        CaptureFilter::SPtr                 m_capture_filter_sptr;
        // properties for upstream request/response
        // ISocketSPtr                         m_upstream_connection_sptr;
        // ClientUPtr                          m_upstream_client_uptr;
        // BufferChain::SPtr                     m_upstream_response_body_sptr;
        // MessageBaseSPtr                     m_upstream_request_sptr;

        // response to downstream client - used for each of the original request types
        // MessageBaseSPtr       m_downstream_response_sptr;
        // obvious - tunnel handler
        // TunnelHandlerSPtr                   m_tunnel_handler_sptr;

};
} // namespace
#endif /* forwarding_handler_hpp */
