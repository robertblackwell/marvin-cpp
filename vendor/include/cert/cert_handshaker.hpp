//
//  handshaker.hpp
//  x509
//
//  Created by ROBERT BLACKWELL on 11/19/17.
//  Copyright © 2017 ROBERT BLACKWELL. All rights reserved.
//

#ifndef certlib_handshaker_handshaker_hpp
#define certlib_handshaker_handshaker_hpp


#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/unordered_set.hpp>

#include "constants.hpp"
#include "version_check.hpp"
#include "x509.hpp"

const int max_read_buffer_length = 1024;
const bool debug_trace = false;

//
// Results from tests are encapsulated in TestResult::value and are inspected  for the correct
// result by a function of the form
//
//      TestResult::validateXXXXX(TestResult::value res, ....)
//
namespace Cert{
namespace Handshaker{
/**
* @brief Provides a type, Handshaker::Result::Value, that is returned by a handshake operation and functiosn to test and manipulate that type
*/
namespace Result {

    typedef Cert::x509::AlternativeDNSNameSet NameSet;

    const NameSet emptySet;
    typedef std::string WhereType;

    const std::string where_read = "READ";
    const std::string where_write = "WRITE";
    const std::string where_handshake = "HANDSHAKE";
    const std::string where_connect = "CONNECT";
    /**
    * \brief Value of result returned by handshake operation. Indicates success or failure
    * and when success carries peer certificate and certificate chain
    */
    struct Value {
    
        Value() :
            m_success(true),
            m_failed_where(where_read),
            m_altNames(emptySet),
            m_pem("")
            {}
        
        inline bool is_success(){ return m_success; };
        inline std::string getPem() {return m_pem;};
        inline std::vector<std::string> getPemChain(){return m_pem_chain;};
        
        friend std::string getPem(Value res);
        friend std::string getPemChain(Value res);
        
        friend Value makeValue(
            bool b,
            std::string s,
            NameSet altnames,
            std::string pem,
            std::vector<std::string> pem_chain
        );
        friend bool validateSuccess( Value res, std::string msg);
        friend bool validateFailInHandshake( Value res, std::string msg);
        friend bool validateSubjectAltNames( Value res, NameSet names, std::string msg);
        // makes the struct immutable
        private:
            bool                    m_success;
            std::string             m_failed_where;
            NameSet                 m_altNames;
            std::string             m_pem;
            std::vector<std::string>             m_pem_chain;
    };
    
    Value makeValue(
        bool b,
        std::string s,
        NameSet altnames,
        std::string pem,
        std::vector<std::string> pem_chain);
    
    bool validateSuccess( Value res, std::string msg);
    bool validateFailInHandshake( Value res, std::string msg);
    
    NameSet intersection(const NameSet &set1, const NameSet &set2);
    
    bool validateSubjectAltNames( Value res, NameSet names, std::string msg);
    std::string getPem(Value res);
    std::string getPemChain(Value res);
} /* namespace Result */
} // namespace Handshaker
} // namespace Cert


namespace Cert{
/**
* \brief Provides functions and type to solicit the x509 certificate from a host/server
*
* \details  The one class in this namespace handshakes with a https host and returns a detailed result
* the handshake.
*
* THe purpose is really only to test the handshake to ensure that
* the client accesses the correct root certificates. Will be used
* to test various scenarios related to which root cetficate store is used
*
* The class client - actually does all the work of setting up the connection,
* handshake, write/read etc. The client appears synchronous from the outside
* but actually uses async io inside via an io_service.
*
* After completion class client constructor returns a TestResult::Value
* so that the test result can be evaluated.
*
* A set of wrapper function around class client are used to parameterize
* the different tests that are needed.
*/
namespace Handshaker {

/**
* \brief Handshakes with a server and returns a handshake result using a pre-existing ssl ctx
* object.
*
* @param server std::string - the server domain name
* @param ctx boost::asio::ssl::content - an ssl context reference
* @return Handshaker::Result::value
*/
Handshaker::Result::Value handshakeWithServer(std::string server, boost::asio::ssl::context& ctx);

/**
* \brief Handshakes with a server and returns a handshake result using a root bundle from a specified file.
* @param server std::string - the server domain name
* @param cert_bundle_file_path  - path to the cert bundle to be used
* @return Handshaker::Result::value
*/
Handshaker::Result::Value handshakeWithServer(std::string server, std::string cert_bundle_file_path);


/**
* \brief Returns the servers certificate as a pem format string
* @param server std::string - the server domain name
* @param ctx boost::asio::ssl::content - an ssl context reference
* @return certificate string in pem format
*/
std::string getServerCertificatePem(std::string server, boost::asio::ssl::context& ctx);
 
 /**
* \brief Returns the servers certificate as a pem format string using a root bundle
* from a specified file.
*
* @param server std::string - the server domain name
* @param cert_bundle_file_path std::string - path to a non default certificate bundle file
* @return certificate string in pem format
*/
std::string getServerCertificatePem(std::string server, std::string cert_bundle_file_path);

//Cert::x509::Identity getServerIdentity(std::string server, std::string cert_bundle_file_path);
/**
* \brief Returns the servers certificate as a pem format string using a pre-existing
* X509_STORE* as the source of the root certificate bundle
*
* @param server std::string - the server domain name
* @param store X509_STORE* - already loaded certificate store structure
* @return certificate string in pem format
*/
std::string getServerCertificatePem(std::string server, X509_STORE *store);
/**
* \brief Returns the servers certificate as a pem format string Using default certificate bundle
* @param server std::string - the server domain name
* @return certificate string in pem format
*/
std::string getServerCertificatePem(std::string server);

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
               boost::asio::ssl::context &ctx,
               boost::asio::io_service& ios
       );
        ~client();
        using HandshakeCallback = std::function<void(boost::system::error_code err)>;
        void handshake(HandshakeCallback cb);
    
        bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

        void extractAltNames();
        void extractCommonNames();
    
        void saveServerCertificate();
    
        void saveServerCertificateChain();
//
        Handshaker::Result::Value getResult();

    private:
    
        void p_handle_connect(const boost::system::error_code& error);
        void p_handle_handshake(const boost::system::error_code& error);
        void p_postCallback(boost::system::error_code err);

        std::string m_port;
        std::string m_server;
        std::string m_request;
        
        std::string m_saved_server_certificate_pem;
        std::vector<std::string> m_pem_saved_certificate_chain;

        boost::asio::io_service& m_ios;
        boost::asio::ssl::context& m_ctx;
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;
    
        bool                        success;
    
        HandshakeCallback           m_handshakeCallback;
    
        Handshaker::Result::WhereType  where;
        Handshaker::Result::NameSet    m_subjectAltnames;
        Handshaker::Result::NameSet    m_commonNames;
        std::string             m_lastCommonName;

};
}; //namespace Handshaker
}; // namespace Cert
#endif /* handshaker_hpp */
