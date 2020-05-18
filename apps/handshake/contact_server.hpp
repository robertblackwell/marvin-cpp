#ifndef marvin_guard_app_handshake_contact_server_hpp
#define marvin_guard_app_handshake_contact_server_hpp
#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <cert/cert.hpp>
#include <marvin/certificates/certificates.hpp>
#include "check_host.cpp"
#include "cert_handshaker.hpp"
#include "marvin_handshaker.hpp"

namespace po = boost::program_options;
namespace bf = boost::filesystem;


struct Result {
    bool  ok;
    std::string host;
    std::string orig_cn;
    std::string orig_san;
};

Result xmitmWorked(X509* original_cert_X509, std::string host, Cert::Identity id)
{
    Cert::Certificate org_cert(original_cert_X509);

    auto orig_spec = Cert_GetSubjectNameAsSpec(original_cert_X509);
    auto orig_cn = (*orig_spec.find(NameNid_commonName)).second;
    
    auto new_spec = Cert_GetSubjectNameAsSpec(id.getX509());
    auto new_cn = (*new_spec.find(NameNid_commonName)).second;

    /// if the host the common name ? - possibly not
    bool b1 = (new_cn == host);
    auto sss = org_cert.getSubjectAlternativeNamesAsString();

    boost::optional<std::string> orig_san = Cert_GetSubjectAlternativeNamesAsString(original_cert_X509);
    std::string orig_san2 = org_cert.getSubjectAlternativeNamesAsString();
    boost::optional<std::string> new_san = Cert_GetSubjectAlternativeNamesAsString(id.getX509());
    std::string orig_san_string {(orig_san) ? orig_san.get(): "NOVALUE"};
    std::string orig_san2_string {orig_san2};
    std::string new_san_string  {(new_san) ? new_san.get(): "NOVALUE"};
    auto xyz = splitDNSNames(orig_san_string);

    bool b2 = (sss.find(host) != std::string::npos);
    bool b3 = matchHostAgainstDnsAltString(host, orig_san2_string);
    bool b4 = matchHostAgainstDnsAltString(host, orig_san_string);
    bool b5 = matchHostAgainstDnsName(host, orig_cn);
    if (! ((b3 && b4)||b5)) {
        std::cout 
        << std::endl << std::endl 
        << "orig_cn " << orig_cn
        << std::endl << std::endl 
        << "orig_san2_string " << orig_san2_string
        << std::endl << std::endl 
        << "orig_san_string " << orig_san_string
        << std::endl << std::endl;

    }
    Result result;
    result.ok = b3 || b5;
    result.host = host;
    result.orig_cn = orig_cn;
    result.orig_san = sss;
    return result;
}

class ContactServer
{
    public:
    bool m_marvin_flag;
    ContactServer(bool marvin_flag)
    {
        m_marvin_flag = marvin_flag;
        // this->loadExisting();
    }

    Result buildMitmCert(std::string host, X509* original_cert_X509)
    {
        Marvin::Certificates certificates = Marvin::Certificates::getInstance();
        Cert::Certificate cert(original_cert_X509);
        Cert::Identity id = certificates.build_server_mitm_certificate(host, cert);
        #ifdef USE_X509
        path store_root = this->storeRootDirPath();
        Cert::Store::LocatorSPtr locator_sptr = std::make_shared<Cert::Store::Locator>(store_root);
        Cert::AuthoritySPtr cert_auth = Cert::Authority::load(locator_sptr->ca_dir_path);
        Cert::Builder builder(*cert_auth);
        Cert::Identity id = builder.buildMitmIdentity(host, cert);
        #endif
        Result mitm_result = xmitmWorked(original_cert_X509, host, id);
        return mitm_result;
        // bool b0 = mitm_result.ok;
        
        // if(b0) {
        //     std::cout << "OK : " << host ;
        // } else {
        //     std::cout << "FAILED : " << host; 
        // }
        // std::cout 
        //     << "original CN:  " << mitm_result.orig_cn 
        //     << "original SAN: " << mitm_result.orig_san
        //     << std::endl; 
    }
    void contactServer(std::string server)
    {
        if (m_marvin_flag) {
            std::cout << "Using marvin handshaker :: " ;
            marvinContactServer(server);
        } else {
            certContactServer(server);
        }
    }
    void certContactServer(std::string server)
    {
        Marvin::Certificates certificats = Marvin::Certificates::getInstance();
        X509_STORE* X509_store_p = certificats.get_X509_STORE_ptr();

        std::shared_ptr<boost::asio::ssl::context> ctx_sptr = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        
        ctx_sptr->set_verify_mode(boost::asio::ssl::verify_peer);
        SSL_CTX_set_cert_store(ctx_sptr->native_handle(), X509_store_p);

        boost::asio::io_service io;
        Marvin::HandshakerV1::client c(
            "https", 
            server, 
            io);
        c.makeSecure();
        c.handshake([this, server, &c](boost::system::error_code err) {
            if (err.failed()) {
                std::cout << "handshake callback : " << server << " err: [" << err.message() << "]" << std::endl;
            } else {
                X509* orig_x509_p = c.m_saved_certificate.native();  
                Result mitm_result = buildMitmCert(server, orig_x509_p);
                X509_free(orig_x509_p);
                bool b0 = mitm_result.ok;
                
                if(b0) {
                    std::cout << "OK : " << server ;
                } else {
                    std::cout << "FAILED : " << server; 
                }
                std::cout 
                    << " original-CN:  " << mitm_result.orig_cn 
                    // << "original SAN: " << mitm_result.orig_san
                    << std::endl; 
            }
        });
        io.run();
    }
    void marvinContactServer(std::string server)
    {
    //     Marvin::Certificates certificats = Marvin::Certificates::getInstance();
    //     X509_STORE* X509_store_p = certificats.get_X509_STORE_ptr();
    //     boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    // #define XTURN_OFF_VERIFY
    // #ifdef TURN_OFF_VERIFY
    //     ctx.set_verify_mode(boost::asio::ssl::verify_none);
    // #else
    //     ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    // #endif
    // #if 0    
    //     std::string moz_only = this->mozRootCertificateBundleFilePath().string();
    //     std::string cert_bundle_path = moz_only;
    // #endif
    //     //
    //     // use a non default root certificate location, AND load them into a custom X509_STORE
    //     //
    //     // attach X509_STORE to boost ssl context
    //     SSL_CTX_set_cert_store(ctx.native_handle(), X509_store_p);


        boost::asio::io_service io;

        ::Marvin::Handshaker handshaker(io, server);

        handshaker.handshake([this, server, &handshaker](boost::system::error_code err) {
            if (err.failed()) {
                std::cout << "handshake callback : " << server << " err: [" << err.message() << "]" << std::endl;
            } else {
                X509* orig_x509_p = handshaker.getServerCertificate().native();  
                Result mitm_result = buildMitmCert(server, orig_x509_p);
                bool b0 = mitm_result.ok;
                X509_free(orig_x509_p);
                
                if(b0) {
                    std::cout << "OK : " << server ;
                } else {
                    std::cout << "FAILED : " << server; 
                }
                std::cout 
                    << " original-CN:  " << mitm_result.orig_cn 
                    // << "original SAN: " << mitm_result.orig_san
                    << std::endl; 
            }
        });
        io.run();
    }
    void handleHostName(int count, std::string host)
    {
        auto xx = host.find("//");
        if (host.find("//") == std::string::npos) {
            std::cout << "handshake "<< count << " with : " << host << " " << std::flush;
            contactServer(host);
        } else {
            std::cout << "SKIPPING handshake with : " << host <<  std::endl;
        }
    }

    void handleFile(std::string file_name)
    {
        bf::path fp{file_name};
        auto xx = bf::canonical(fp);
        if (!bf::is_regular_file(fp)) {
            std::cout << "file : " << file_name << " is not a file or does not exist" << xx << std::endl;
        }
        std::fstream fs{file_name};
        if (!fs) {
            std::cout << "file : " << file_name << " could not open" << std::endl;
        }
        std::string line;
        int i = 1;
        while (getline (fs, line)) {
            // std::cout << line << std::endl;
            handleHostName(i, line);
            i++;
        }
    }
};
#endif