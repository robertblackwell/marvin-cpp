//
//  handshaker.hpp
//  x509
//
//  Created by ROBERT BLACKWELL on 11/19/17.
//  Copyright © 2017 ROBERT BLACKWELL. All rights reserved.
//

#ifndef marvin_cert_version_handshaker_handshaker_hpp
#define marvin_cert_version_handshaker_handshaker_hpp


#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/unordered_set.hpp>

#include <cert/constants.hpp>
#include <cert/version_check.hpp>
#include <cert/x509.hpp>
#include <cert/cert_certificate.hpp>
#include <cert/cert_chain.hpp>

namespace Marvin{
namespace HandshakerV1 {

/**
* \brief This class performs a TLS handshake with a server and collects the servers certificate (chain)
* and makes it available after the handshake is complete.
*
* The client uses async io with boost::asio BUT the interfaces/convenience functions above all act synchronously
* by using a private io_service.
*
* THe client classs is exposed to enable full async operation of it is required
*/
class client
{
    public:
        client(
               std::string port,
               std::string server,
               std::shared_ptr<boost::asio::ssl::context> ctx_sptr,
               boost::asio::io_service& ios
       );
        ~client();
        using HandshakeCallback = std::function<void(boost::system::error_code err)>;
        void makeSecure();
        void handshake(HandshakeCallback cb);
    
        bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

        void extractAltNames();
        void extractCommonNames();
    
        void saveServerCertificate();
    
        void saveServerCertificateChain();

    public:
    
        void p_handle_connect(const boost::system::error_code& error);
        void p_handle_handshake(const boost::system::error_code& error);
        void p_postCallback(boost::system::error_code err);

        std::string m_port;
        std::string m_server;
        std::string m_request;
        std::string m_saved_server_certificate_pem;
        Certificate m_saved_certificate;
        X509* m_raw_x509_p;
        
        std::vector<std::string> m_pem_saved_certificate_chain;
        CertChain   m_saved_certificate_chain;
        STACK_OF(X509*) m_raw_stack_x509;

        boost::asio::io_service& m_ios;
        std::shared_ptr<boost::asio::ssl::context>                              m_ssl_ctx_sptr;   
        std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> m_socket_sptr;
    
        bool                   success;
    
        HandshakeCallback      m_handshakeCallback;
   
        std::string            m_lastCommonName;

};
}; //namespace Handshaker
}; // namespace Cert
#endif /* handshaker_hpp */
