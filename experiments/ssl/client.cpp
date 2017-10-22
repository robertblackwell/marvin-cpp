//
// test_cert_file.cpp
// ~~~~~~~~~~
// The purpose of this program is to runs a series of test the correct usage of various cert files.
//
// One set of test involves trying to connect to, handshake with and perform a "GET /" request
// with a know TLS/SSL domain (ssllabs) using either the openssl default cert file or a
// nominated cert file.
//
// The second set of tests involves trying to connect to, handshake with and perform a GET request
// to a local domain which uses a certificate signed by custom (not well known) self signed root.
//
// For most tests the process should run successfully to completion with a response to the GET request
// being received. But for some tests, where the cert file deliberately does not contain the root certificate used
// by target domain, the test should fail during the handshake stage of the process.
//
// The first set of tests uses a known public domain http://www.ssllabs.com
//
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "connection_interface.hpp"
#include "tls_connection.hpp"

const int max_read_buffer_length = 1024;
const bool debug_trace = false;


namespace TestResult {
    typedef std::string WhereType;
    const std::string where_read = "READ";
    const std::string where_write = "WRITE";
    const std::string where_handshake = "HANDSHAKE";
    const std::string where_connect = "CONNECT";
    typedef std::tuple<bool, std::string> Value;
    Value makeValue(bool b, std::string s) {
        return std::make_tuple(b, s);
    }
    void validateSuccess( Value res, std::string msg)
    {
        bool val;
        std::string s;
        tie(val, s) = res;

        if (! val ) {
            throw "expectSuccess failed " + msg;
        }
        std::cout << "Test Passed " << msg << std::endl;
    }
    void validateFailInHandshake( Value res, std::string msg)
    {
        bool val;
        std::string s;
        tie(val, s) = res;
        if ( val ) {
            throw "expectFailInHandshake failed got success " + msg;
        }
        if ( !val && (s != where_handshake) ) {
            throw "expectFailInHandshake failed not in handshake but in  " + s + " "  + msg;
        }
        std::cout << "Test Passed " << msg << std::endl;
    }

} /* namespace RestResult */

class client
{
    public:
        client(
            boost::asio::io_service& io_service,
            boost::asio::ssl::context& context,
            boost::asio::ip::tcp::resolver::iterator endpoints
        ) : socket_(io_service, context), success(false)
        {
//            socket_.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));

            boost::asio::async_connect(
                socket_.lowest_layer(),
                endpoints,
                boost::bind(&client::handle_connect, this, boost::asio::placeholders::error)
            );
        }

    bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "XXVerifying " << subject_name << "\n";
        std::cout << "Preverified " << preverified << std::endl;
        char* fn = (char*)X509_get_default_cert_file();
        char* dn = (char*)X509_get_default_cert_dir();
        char* n1 = (char*)X509_get_default_cert_area();
        char* n2 = (char*)X509_get_default_cert_dir_env();
        char* n3 = (char*)X509_get_default_cert_file_env();

        //
        // check the server has a certificate
        //
        X509* server_cert = SSL_get_peer_certificate(socket_.native_handle());
//        if(cert) { X509_free(cert); }
//        if(NULL == cert) handleFailure();

        /* get a "list" of alternative names */
        STACK_OF(GENERAL_NAME) *altnames;
        altnames = (STACK_OF(GENERAL_NAME)*) X509_get_ext_d2i(server_cert, NID_subject_alt_name, NULL, NULL);


        return preverified ;
    }

    void handle_connect(const boost::system::error_code& error)
    {
        if (!error) {
            socket_.async_handshake(
                boost::asio::ssl::stream_base::client,
                boost::bind(&client::handle_handshake, this, boost::asio::placeholders::error)
            );
        }else{
            this->where = TestResult::where_connect;
            if (debug_trace) std::cout << "Connect failed: " << error.message() << "\n";
        }
    }
    void scanAltNames()
    {
        X509* server_cert = SSL_get_peer_certificate(socket_.native_handle());
        STACK_OF(GENERAL_NAME) *altnames;
        altnames = (STACK_OF(GENERAL_NAME)*) X509_get_ext_d2i(server_cert, NID_subject_alt_name, NULL, NULL);
        if(!altnames)
            return;
        int numalts;
        int i;

        /* get amount of alternatives, RFC2459 claims there MUST be at least
           one, but we don't depend on it... */
        numalts = sk_GENERAL_NAME_num(altnames);

        /* loop through all alternatives while none has matched */
        for (i=0; (i<numalts); i++)
        {
            /* get a handle to alternative name number i */
            const GENERAL_NAME *check = sk_GENERAL_NAME_value(altnames, i);
            /* get data and length */
            const char *altptr = (char *)ASN1_STRING_data(check->d.ia5);
            size_t altlen = (size_t) ASN1_STRING_length(check->d.ia5);
            
            std::string s(altptr);
            std::cout << s << std::endl;
        }
//        GENERAL_NAMES_free(altnames);
    }

    void handle_handshake(const boost::system::error_code& error)
    {
        if (!error) {
            scanAltNames();
//            const char* tmp = "GET https://ssllabs.com/ HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
            const char* tmp = "GET / HTTP/1.1\r\nHOst: www.ssllabs.com\r\n\r\n";
            strcpy(request_, tmp);
            size_t request_length = strlen(request_);

            boost::asio::async_write(
                socket_,
                boost::asio::buffer(request_, request_length),
                boost::bind(
                    &client::handle_write,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            );
        } else {
            this->where = TestResult::where_handshake;
            if (debug_trace) std::cout << "Handshake failed: " << error.message() << "\n";
        }
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error){
            boost::asio::async_read(
                socket_,
                boost::asio::buffer(reply_, bytes_transferred),
                boost::bind(
                    &client::handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            );
        }else{
            this->where = TestResult::where_write;
            if (debug_trace) std::cout << "Write failed: " << error.message() << "\n";
        }
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if( !error ) {
            if (debug_trace || true) {
                std::cout << "Reply: ";
                std::cout.write(reply_, bytes_transferred);
                std::cout << "\n";
            }
            this->success = true;
        }else{
            this->where = TestResult::where_read;
            if (debug_trace) std::cout << "Read failed: " << error.message() << "\n";
        }
    }
    TestResult::Value getResult()
    {
        return TestResult::makeValue(this->success, this->where);
    }
    
//    bool wasSuccessFul()
//    {
//        return this->success;
//    }
//
//    std::string failedWhere()
//    {
//        return this->where;
//    }
//
    private:
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
        bool success;
        TestResult::WhereType where;
        char request_[max_read_buffer_length];
        char reply_[max_read_buffer_length];
};


//
// Runs a single test and returns a tuple
//  -   first element of the tuple is a bool
//      -   true means the GET request succeeded in which case the second element is meaningless
//      -   false means the process failed somewher, the second element is a string telling at what
//          step it failed, values are
//              CONNECT, HANDSHAKE, WRITE, READ
//
//
TestResult::Value
runOneTest(std::string server, bool useDefaultCertPath, std::string certFilePath)
{
    std::string port("https");
    boost::asio::io_service ios;
    boost::asio::ip::tcp::resolver resolver(ios);
    boost::asio::ip::tcp::resolver::query query(server, port);
    boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);
    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    if(useDefaultCertPath) {
        ctx.set_default_verify_paths();
    } else {
        ctx.load_verify_file(certFilePath);
    }
    client c(ios, ctx, endpoints);
    ios.run();
    return c.getResult();
//    return std::make_tuple(c.wasSuccessFul(), c.failedWhere());
}

TestResult::Value
withDefaultCertFile(std::string server)
{
    return runOneTest(server, true, "");
};

TestResult::Value
withNonDefaultCertFile(std::string server, std::string certFilePath)
{
    return runOneTest(server, false, certFilePath);
};

namespace ssllabs {
    void testSuiteSslLabs()
    {
        TestResult::Value res;

        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
        res = withDefaultCertFile("www.ssllabs.com");
        TestResult::validateSuccess(res, "ssllabs + default");

        //
        // this one should work as is explicitly points at the default
        //
        res = withNonDefaultCertFile("www.ssllabs.com", "/usr/local/ssl/cert.pem");
        TestResult::validateSuccess(res, "ssllabs + explicit use of default file");

        //
        // this one should work as ssllabs uses Entrust as a root CA and that root Cert is in
        // the given pem file
        //
        res = withNonDefaultCertFile("www.ssllabs.com",
              "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/cacert.pem");
        TestResult::validateSuccess(res, "ssllabs + local cert file that has the required root");

        //
        // this one should return false as Entrust's root certs have been removed from x_cacrt.pem
        //
        res = withNonDefaultCertFile("www.ssllabs.com",
                   "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/x_cacert.pem");
        TestResult::validateFailInHandshake(res, "ssllabs + local cert file that DOES NOT have required root cert");
    }
}
namespace privateCA {
    void testSuite() {
        TestResult::Value res;

        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
        res = withDefaultCertFile("ssltest");
        TestResult::validateFailInHandshake(res, "ssltest + default");

        //
        // this one should work as is explicitly points at the default
        //
        res = withNonDefaultCertFile("ssltest", "/usr/local/ssl/cert.pem");
        TestResult::validateFailInHandshake(res, "ssltest + explicit use of default file");
        //
        // this one should return work because even though Entrust's root certs have been removed from x_cacrt.pem
        // this file includes the CERT for BlackwellApps which is the root
        //
        res = withNonDefaultCertFile("ssltest",
                   "/Users/rob/CA/allroots/osx-allcerts.pem");
        TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root");

        res = withNonDefaultCertFile("ssltest",
                   "/Users/rob/CA/allroots/combined-cacert.pem");
        TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root");

    };
    TestResult::Value testOne() {
        return TestResult::makeValue(true,"");
    }
}
int main(int argc, char* argv[])
{
    ssllabs::testSuiteSslLabs();
    privateCA::testSuite();
    return 0;
}
