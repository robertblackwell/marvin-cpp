//
// test_cert_file.cpp
// ~~~~~~~~~~
// The purpose of this program is to runs a series to test the correct usage of various cert files.
//
// One set of test involves trying to connect to, handshake with and perform a "GET /" request
// with a know TLS/SSL domain (ssllabs) using either the openssl default cert file or a
// nominated cert file.
//
// The second set of tests involves trying to connect to, handshake with and perform a GET request
// to a local domain (ssltest) which uses a certificate signed by custom (not well known) self signed root.
//
// For most tests the process should run successfully to completion with a response to the GET request
// being received. But for some tests, where the cert file deliberately does not contain the root certificate used
// by target domain, the test should fail during the handshake stage of the process.
//
// For the ssltest suite of tests we also demonstrate how to extract subjectAltNames from the service certificate
// and verify that we found exactly the alt names that were built into the service certificate.
//
// NOte : each test has two variations:
//  -   one, where the root certificates are loaded directly into the SSL ctx
//  -   one, where the root certificates are loaded into an X509_STORE which is then hooked to the SSL ctx.
//      this test ensures that we can load the root store from a file only once and re-use it for multiple requests
//

/**
**
** the stuff below is only a reference regardin how to use X509_STORE
**
*/
#ifdef HGHGHGHG

#include <boost/asio/ssl/context.hpp>
#include <wincrypt.h>

void add_windows_root_certs(boost::asio::ssl::context &ctx)
{
    HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
    if (hStore == NULL) {
        return;
    }

    X509_STORE *store = X509_STORE_new();
    PCCERT_CONTEXT pContext = NULL;
    while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
        // convert from DER to internal format
        X509 *x509 = d2i_X509(NULL,
                              (const unsigned char **)&pContext->pbCertEncoded,
                              pContext->cbCertEncoded);
        if(x509 != NULL) {
            X509_STORE_add_cert(store, x509);
            X509_free(x509);
        }
    }

    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    // attach X509_STORE to boost ssl context
    SSL_CTX_set_cert_store(ctx.native_handle(), store);
}

This will load the certificates from the windows ca store (similar to the question you linked). But instead of converting the certificates to base64 it uses d2i_X509 to convert them to the internal OpenSSL format and adds them to an OpenSSL X509_STORE. Then SSL_CTX_set_cert_store attaches that store to the boost ssl context. You can use that to set up your ssl context and then use that for the ssl socket:

namespace ssl = boost::asio::ssl;
ssl::context ctx(ssl::context::tlsv12_client);
ctx.set_options(ssl::context::default_workarounds
                            | ssl::context::no_sslv2
                            | ssl::context::no_sslv3
                            | ssl::context::tlsv12_client);

add_windows_root_certs(ctx);
ctx.set_verify_mode(ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);

// use custom verify_callback here for debugging purposes
ctx.set_verify_callback(ssl::rfc2818_verification(address));

ssl::stream<boost::asio::ip::tcp::socket> socket(io, ctx);
// socket ready to connect to ssl host

#endif

#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/unordered_set.hpp>
#include "connection_interface.hpp"
#include "tls_connection.hpp"

const int max_read_buffer_length = 1024;
const bool debug_trace = false;

//
// Results from tests are encapsulated in TestResult::value and are inspected  for the correct
// result by a function of the form
//
//      TestResult::validateXXXXX(TestResult::value res, ....)
//
namespace TestResult {
    typedef boost::unordered::unordered_set<std::string> NameSet;
    const NameSet emptySet;
    typedef std::string WhereType;

    const std::string where_read = "READ";
    const std::string where_write = "WRITE";
    const std::string where_handshake = "HANDSHAKE";
    const std::string where_connect = "CONNECT";

    struct Value {
    
        Value() : _success(true), _failed_where(where_read), _altNames(emptySet) {}
        friend Value makeValue(bool b, std::string s, NameSet altnames);
        friend void validateSuccess( Value res, std::string msg);
        friend void validateFailInHandshake( Value res, std::string msg);
        friend void validateSubjectAltNames( Value res, NameSet names, std::string msg);

        // makes the struct immutable
        private:
            bool                    _success;
            std::string             _failed_where;
            NameSet                _altNames;
    };
    
    Value makeValue(bool b, std::string s, NameSet altnames) {
        Value x;
        x._success = b;
        x._failed_where = s;
        x._altNames = altnames;
        return x;
    }
    
    void validateSuccess( Value res, std::string msg)
    {
        if (! res._success ) {
            throw "expectSuccess failed " + msg;
        }
        std::cout << "Test Passed - success" << msg << std::endl;
    }
    void validateFailInHandshake( Value res, std::string msg)
    {
        if ( res._success ) {
            throw "expectFailInHandshake failed got success " + msg;
        }
        if ( !res._success && (res._failed_where != where_handshake) ) {
            throw "expectFailInHandshake failed not in handshake but in  " + res._failed_where + " "  + msg;
        }
        std::cout << "Test Passed - failed handshake " << msg << std::endl;
    }
    
    NameSet intersection(const NameSet &set1, const NameSet &set2){
        if(set1.size() <= set2.size()){

            NameSet iSet;

            boost::unordered_set<std::string>::iterator it;
            for(it = set1.begin(); it != set1.end();++it){
                if(set2.find(*it) != set2.end()){
                    iSet.insert(*it);
                }
            }
            return iSet;
        }else{
            return intersection(set2,set1);
        }
    }
    
    void validateSubjectAltNames( Value res, NameSet names, std::string msg)
    {
        if( ! res._success )
            throw "verifySubjectAltNames failed " + msg;
        if( res._altNames.size() != names.size() )
            throw "verifySubjectAltNames names mismatch(size) " + msg;
        auto tmpSet = intersection(res._altNames, names);
        if( res._altNames.size() != tmpSet.size() )
            throw "verifySubjectAltNames names mismatch(intersection) " + msg;
        std::cout << "Test Passed - subject alternate names " << msg << std::endl;

    }

} /* namespace RestResult */

//
// namespace TestCase
// ==================
// - contains the code that actually runs a single test.
//
// the class client - actually does all the work of setting up the connection,
// handshake, write/read etc. The client appears synchronous from the outside
// but actually uses async io inside via an io_service.
//
// After completion class client constructor returns a TestResult::Value
// so that the test result can be evaluated.
//
// A set of wrapper function around class client are used to parameterize
// the different tests that are needed.
//
namespace TestCase {
    class client
    {
        public:
        client(
               std::string port,
               std::string server,
               std::string request,
               boost::asio::ssl::context &ctx)
         :  _port(port), _server(server), _request(request), _ctx(ctx),_socket(_ios, _ctx), success(false)
        {
            _ctx.set_verify_mode(boost::asio::ssl::verify_peer);
            boost::asio::ip::tcp::resolver resolver(_ios);
            boost::asio::ip::tcp::resolver::query query(server, port);
            boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);
    //            socket_.set_verify_callback(boost::bind(&client::verify_certificate, this, _1, _2));
            boost::asio::async_connect(
                _socket.lowest_layer(),
                endpoints,
                boost::bind(&client::handle_connect, this, boost::asio::placeholders::error)
            );
            _ios.run();
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
            X509* server_cert = SSL_get_peer_certificate(_socket.native_handle());
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
                _socket.async_handshake(
                    boost::asio::ssl::stream_base::client,
                    boost::bind(&client::handle_handshake, this, boost::asio::placeholders::error)
                );
            }else{
                this->where = TestResult::where_connect;
                if (debug_trace) std::cout << "Connect failed: " << error.message() << "\n";
            }
        }
        void extractAltNames()
        {
            X509* server_cert = SSL_get_peer_certificate(_socket.native_handle());
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
                _subjectAltnames.insert(s);
                if (debug_trace) std::cout << s << std::endl;
            }
            GENERAL_NAMES_free(altnames);
        }
        void extractCommonNames()
        {
            std::vector<std::string> v;
            std::map<std::string, std::string> m;
            auto xctx = _socket.native_handle();
            X509* cert = SSL_get_peer_certificate(_socket.native_handle());

            X509_NAME* name = X509_get_subject_name(cert);
            int i = -1;
            ASN1_STRING* common_name = 0;
            std::string commonNameString;
            while ((i = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
            {
                X509_NAME_ENTRY* name_entry = X509_NAME_get_entry(name, i);
                common_name = X509_NAME_ENTRY_get_data(name_entry);
                std::string sname((char*)common_name->data, (int)common_name->length);
                _commonNames.insert(sname);
                commonNameString = sname;
                v.push_back(sname);
                m[sname] = sname;
            }
            if (common_name && common_name->data && common_name->length)
            {
                _lastCommonName = commonNameString;
//                const char* pattern = reinterpret_cast<const char*>(common_name->data);
//                std::size_t pattern_length = common_name->length;
//                if (match_pattern(pattern, pattern_length, host_.c_str()))
//                return true;
            }
        }
        
        void handle_handshake(const boost::system::error_code& error)
        {
            if (!error) {
                extractAltNames();
                extractCommonNames();
                std::string r = _request + " HTTP/1.1 \r\n" + "Host: " + _server + "\r\n\r\n";
    //            const char* tmp = "GET https://ssllabs.com/ HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
                const char* tmp = "GET / HTTP/1.1\r\nHost: www.ssllabs.com\r\n\r\n";
                strcpy(_requestBuffer, tmp);
                strcpy(_requestBuffer, r.c_str());
                size_t request_length = strlen(_requestBuffer
                );

                boost::asio::async_write(
                    _socket,
                    boost::asio::buffer(_requestBuffer, request_length),
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
                    _socket,
                    boost::asio::buffer(_replyBuffer, bytes_transferred),
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
                if (debug_trace) {
                    std::cout << "Reply: ";
                    std::cout.write(_replyBuffer, bytes_transferred);
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
            return TestResult::makeValue(this->success, this->where, this->_subjectAltnames);
        }
        private:
            std::string _port;
            std::string _server;
            std::string _request;

            boost::asio::io_service _ios;
            boost::asio::ssl::context& _ctx;
            boost::asio::ssl::stream<boost::asio::ip::tcp::socket> _socket;
        
            bool                    success;
            TestResult::WhereType   where;
            TestResult::NameSet     _subjectAltnames;
            TestResult::NameSet     _commonNames;
            std::string             _lastCommonName;

            char _requestBuffer[max_read_buffer_length];
            char _replyBuffer[max_read_buffer_length];
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
    runOneTest(std::string server, bool useDefaultCertPath, std::string certFilePath,  bool viaX509Store = false)
    {
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.set_verify_mode(boost::asio::ssl::verify_peer);
        if(useDefaultCertPath) {
            if( ! viaX509Store) {
                ctx.set_default_verify_paths();
            } else {
                X509_STORE *store = X509_STORE_new();
                X509_STORE_set_default_paths(store);
                // attach X509_STORE to boost ssl context
                SSL_CTX_set_cert_store(ctx.native_handle(), store);
            }
        } else {
            if( ! viaX509Store) {
                ctx.load_verify_file(certFilePath);
            } else {
                X509_STORE *store = X509_STORE_new();
                X509_STORE_load_locations(store, (const char*)certFilePath.c_str(), NULL);
                // attach X509_STORE to boost ssl context
                SSL_CTX_set_cert_store(ctx.native_handle(), store);
            }
        }
        client c("https", server, "GET /", ctx);
        return c.getResult();
    }

    TestResult::Value
    withDefaultCertFile(std::string server)
    {
        return runOneTest(server, true, "", false);
    };
    TestResult::Value
    withDefaultCertFileViaX509Store(std::string server)
    {
        return runOneTest(server, true, "", true);
    };

    TestResult::Value
    withNonDefaultCertFile(std::string server, std::string certFilePath)
    {
        return runOneTest(server, false, certFilePath, true);
    };
    TestResult::Value
    withNonDefaultCertFileViaX509Store(std::string server, std::string certFilePath)
    {
        return runOneTest(server, false, certFilePath, true);
    };
} // namespace TestCase

//
// namespace - ssllabs
// ===================
//
// Wraps a series of tests that make connection/requests to https://ssllabs.com
//
namespace ssllabs {
    void testSuite()
    {
        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
        auto res = TestCase::withDefaultCertFile("www.ssllabs.com");
        TestResult::validateSuccess(res, "ssllabs + default");

        //
        // this one should work as is explicitly points at the default
        //
        res = TestCase::withNonDefaultCertFile("www.ssllabs.com", "/usr/local/ssl/cert.pem");
        TestResult::validateSuccess(res, "ssllabs + explicit use of default file");

        //
        // this one should work as ssllabs uses Entrust as a root CA and that root Cert is in
        // the given pem file
        //
        res = TestCase::withNonDefaultCertFile("www.ssllabs.com",
              "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/cacert.pem");
        TestResult::validateSuccess(res, "ssllabs + local cert file that has the required root");
        //
        // this one should return false as Entrust's root certs have been removed from x_cacrt.pem
        //
        res = TestCase::withNonDefaultCertFile("www.ssllabs.com",
                   "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/x_cacert.pem");
        TestResult::validateFailInHandshake(res, "ssllabs + local cert file that DOES NOT have required root cert");
    }
    void testSuiteViaX509Store()
    {
        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
        auto res = TestCase::withDefaultCertFileViaX509Store("www.ssllabs.com");
        TestResult::validateSuccess(res, "ssllabs + default + x509 store");

        //
        // this one should work as is explicitly points at the default
        //
        res = TestCase::withNonDefaultCertFileViaX509Store("www.ssllabs.com", "/usr/local/ssl/cert.pem");
        TestResult::validateSuccess(res, "ssllabs + explicit use of default file + x509 store");

        //
        // this one should work as ssllabs uses Entrust as a root CA and that root Cert is in
        // the given pem file
        //
        res = TestCase::withNonDefaultCertFileViaX509Store("www.ssllabs.com",
              "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/cacert.pem");
        TestResult::validateSuccess(res, "ssllabs + local cert file that has the required root + x509 store");
        //
        // this one should return false as Entrust's root certs have been removed from x_cacrt.pem
        //
        res = TestCase::withNonDefaultCertFileViaX509Store("www.ssllabs.com",
                   "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/experiments/ssl/x_cacert.pem");
        TestResult::validateFailInHandshake(res, "ssllabs + local cert file that DOES NOT have required root cert + x509 store");
    }

} // namespace ssllabs

//
// namespace - ssltest
// ===================
//
// Wraps a series of tests that make connection/requests to a locally running website
// called  https://ssltest
//
// This is a site where I have control of the certificate. Hence this suite not only tests this code
// but also the process I use for generating server certificates signed by a custom "self signed"
// root.
//
// eventually see github.com/robertblackwell/CA.git for the certificate management process
//
namespace ssltest {
    void testSuite() {
        TestResult::Value res;
        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
            res = TestCase::withDefaultCertFile("ssltest");
            TestResult::validateFailInHandshake(res, "ssltest + default");
        //
        // this one should work as is explicitly points at the default
        //
            res = TestCase::withNonDefaultCertFile("ssltest", "/usr/local/ssl/cert.pem");
            TestResult::validateFailInHandshake(res, "ssltest + explicit use of default file");
        //
        // this one should return work because even though Entrust's root certs have been removed from x_cacrt.pem
        // this file includes the CERT for BlackwellApps which is the root
        //
            res = TestCase::withNonDefaultCertFile("ssltest",
                       "/Users/rob/CA/allroots/osx-allcerts.pem");
            TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root");

            res = TestCase::withNonDefaultCertFile("ssltest",
                       "/Users/rob/CA/allroots/combined-cacert.pem");
            TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root");
            TestResult::NameSet s;
            s.insert("ssltest");
            s.insert("api.ssltest");
            s.insert("www.ssltest");
            TestResult::validateSubjectAltNames(res, s, "ssltest + testing subjectAltNames");
    };
    void testSuiteViaX509Store() {
        TestResult::Value res;
        //
        // uses default, which is /usr/local/ssl/cert.pem
        //
            res = TestCase::withDefaultCertFileViaX509Store("ssltest");
            TestResult::validateFailInHandshake(res, "ssltest + default + x509 store");
        //
        // this one should work as is explicitly points at the default
        //
            res = TestCase::withNonDefaultCertFileViaX509Store("ssltest", "/usr/local/ssl/cert.pem");
            TestResult::validateFailInHandshake(res, "ssltest + explicit use of default file + x509 store");
        //
        // this one should return work because even though Entrust's root certs have been removed from x_cacrt.pem
        // this file includes the CERT for BlackwellApps which is the root
        //
            res = TestCase::withNonDefaultCertFileViaX509Store("ssltest",
                       "/Users/rob/CA/allroots/osx-allcerts.pem");
            TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root + x509 store");

            res = TestCase::withNonDefaultCertFileViaX509Store("ssltest",
                       "/Users/rob/CA/allroots/combined-cacert.pem");
            TestResult::validateSuccess(res, "ssltest + local cert file that has BlackwellApps root + x509 store");
            TestResult::NameSet s;
            s.insert("ssltest");
            s.insert("api.ssltest");
            s.insert("www.ssltest");
            TestResult::validateSubjectAltNames(res, s, "ssltest + testing subjectAltNames");
    };

    TestResult::Value testOne() {
        TestResult::Value x= TestResult::makeValue(true,"zzzz", TestResult::emptySet);
        return x;
    }
} // namespace ssltest

int main(int argc, char* argv[])
{
    
    ssllabs::testSuite();
    ssllabs::testSuiteViaX509Store();
    ssltest::testSuite();
    ssltest::testSuiteViaX509Store();

    return 0;
}
