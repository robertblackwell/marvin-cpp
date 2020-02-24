/**
 * header file for Cert Chai and Cert Bundle Functions
 */
#ifndef cert_x509_chain_include_hpp
#define cert_x509_chain_include_hpp
namespace Cert {
    class Authority;
}

namespace Cert {
namespace x509 {
    /**
    * \brief a cert chain and a cert bundle is an ordered list of certificates in PEM
    * string format.
    * For cert chains the order matters, these are what a server sends to client
    * during handshake. For cert bundles the order does not matter - these are bundles of root
    * certificates used to verigy a host certificate(chain).
    */
    typedef std::vector <std::string> CertChain;
    typedef std::vector <std::string> CertBundle;

    CertBundle CertChain_FromString(std::string pem_string);

    /**
    * Read a cert bundle file into a vector of strings in pem format
    *
    * @param filename std::string full path to file holding certificate bundle in pem format
    * @return std::vector<std::string> vector of PEM format strings
    */
    CertBundle CertChain_FromFile(std::string filename);
    /**
    * Create a CertChain instance from a STACK of X509*
    */
    CertChain CertChain_FromStack(STACK_OF(X509)* cert_stack);
    /**
    * Create an empty certificate chain
    */
    CertChain CertChain_empty();

    /**
    * Returns a PEM certificate chain as a single string
    */
    std::string CertChain_ToString(CertChain& cert_chain);
    
    /**
    * Write a certificate chain to a file in PEM format
    */
    void CertChain_WriteToFile(CertChain& chain , std::string filename);

    void CertChain_WriteToFile(STACK_OF(X509)* chain , std::string filename);

    void CertChain_Print(CertChain& chain, BIO* out_bio);

    void CertChain_Print(CertChain& chain);

    
} // namespace x509
} //namespace Cert
#endif
