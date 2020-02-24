#ifndef certlib_cert_oo_certificate_hpp
#define certlib_cert_oo_certificate_hpp
#include <boost/filesystem/path.hpp>
#include "constants.hpp"
#include "version_check.hpp"
#include "x509.hpp"


using namespace Cert::x509;

namespace Cert {
class EvpPKey;
class Certificate;

class Certificate
{
public:
private:
    X509*   m_x509;
public:
    #pragma mark - certificate no move/copy or assign
    Certificate() = delete;

    Certificate(const Certificate& other) = default;
    Certificate & operator=(const Certificate&) = default;
    /**
    * Read an X509* certificate from a file in PEM format
    * @throws exception on failure
    */
	Certificate(boost::filesystem::path pem_file);
    /**
    * Read an X509* certificate from a string in PEM format
    * @throws exception on failure
    */
	Certificate(std::string pem_string);
    /**
    * Construct from an existing X509* certificate
    * @throws exception on failure
    */
	Certificate(X509* x509_cert);

    ~Certificate();
    
    // The pointer returned by this method is owned by the class instance. Do not try
    // and free it.
    X509* native();
    
    /**
    * Writes a certificate in PEM format to a file
    * @throws if any error occurs
    */
    void writeToFile(boost::filesystem::path  filePath);
#pragma mark - certificate subject_name getters/setters
    /**
     * Get a certificates subject_name as a C++ datastructure,
     *
     * @return     Cert::x509::NameSpecification. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    ::Cert::x509::NameSpecification getSubjectNameAsSpec();
    /**
     * Get a certificates subject_name in summary form as a string,
     *
     * @return     std::string. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    std::string getIssuerNameAsOneLine();
    /**
     * @brief     Get a certificates subject_name in detailed form as a string,
     *
     * @return     std::string. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    std::string getIssuerNameAsMultiLine();

    
    /**
     * @brief      Set the value of a certificates subject_name
     *              using a Cert::x509::NameSpecification as the definition
     *              of what the name will contain. 
     *
     * @param[in]  subjectValues  The subject name component values
     * 
     * @throws an exception on any failure
     */
    void setSubjectName(::Cert::x509::NameSpecification subjectValues);


#pragma mark - certificate issuer_name getters/setters

    /**
     * @brief     Get a certificates issuer_name as a C++ datastructure,
     *
     * @param      cert  The cert, X509*
     *
     * @return     Cert::x509::NameSpecification. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    ::Cert::x509::NameSpecification getIssuerNameAsSpec();

    /**
     * @brief     Get a certificates issuer_name in summary form as a string,
     *
     * @return     std::string. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    std::string getSubjectNameAsOneLine();
    /**
     * @brief     Get a certificates issuer_name in detailed form as a string,
     *
     * @return     std::string. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    std::string getSubjectNameAsMultiLine();

    /**
     * @brief      Set the value of a certificates issuer_name
     *              using a Cert::x509::NameSpecification ass the definition
     *              of what the name will contain. 
     *
     * @param      cert           The cert X509*
     * @param[in]  spec         The component values of the subject_name to be built
     * @throws an exception on any failure
     */
    void setIssuerName(::Cert::x509::NameSpecification spec);

#pragma mark - certificate version getters/setters

    /**
    * Gets the version of a certificate
     * 
     * @return Cert::x509::Verions  - this is a by value return
     * 
     * @throws an exception on any failure
    */
    ::Cert::x509::Version getVersion();

    /**
     * Sets the value of the version in a certificate.
     * 
     * @param version Cert::x509::Version
     *
     * This works   Cert_SetVersion(cert2, Cert_GetVersioine(cert1))
     *
     * @throws an exception on any failure
    */
    void setVersion(::Cert::x509::Version version);

#pragma mark - certificate serial number getters/setters

    std::string getSerialNumber();
    /**
     * Sets the value of the serial number in a certificate.
     * @param serial an Cert::x509::Serial - 
     *
     * @throws an exception on any failure
    */
    void setSerialNumber(::Cert::x509::SerialNumber serial);

#pragma mark - certificate public key getters/setters

    EvpPKey getPublicKey();

    void setPublicKey(EvpPKey pubkey);

#pragma mark - certificate not before and not after getters/setters

    void setNotBefore(int offset_from_now_secs);

    void setNotAfter(int offset_from_now_secs);

#pragma mark - extension functions
    /**
    * @brief    Gets a set of subject alternative names from a certificate's subject_alt_names extensions
    *
    * @return a std::string
    */
    std::string getSubjectAlternativeNamesAsString();
    /**
    * @brief    Gets a set of DNS names from a certificate's subject_alt_names extensions
    *
    * @return AlternativeDNSNameSet
    */
    AlternativeDNSNameSet getSubjectAlternativeDNSNames();

    /**
     * @brief      This function gets all the extensions from a certificate and returns
     *              in a C++ data structure that is ONLY useful for display. The returned
     *              value CANNOT be used to set the extensions of the same or a different
     *              certificate. 
     *
     * @return     extensions values as Cert::x509::ExtDescriptions 
     */
    ::Cert::x509::ExtDescriptions getExtensionsAsDescription();

    /**
     * @brief      Sets the value of the extensions for a certificate. The value to
     *              be given to each certificate is determined by the openssl/crypto code
     *              based on the contents of the Cert::x509::ExtSpecifications object
     *              but in a number of cases the value set is NOT the same as that given in the
     *              specification. see 
     *              //http://openssl.cs.utah.edu/docs/apps/x509v3_config.html#certificate_policies_
     *
     *              Note : that some certificate extension values require information from the
     *              certificate of the certificate authority that will sign the certificate whose
     *              extensions are being set.
     *              
     * @param      ca_cert           The ca cert
     * @param      cert              The cert
     * @param[in]  extra_extensions  The extra extensions
     */
    void addExtensionsFromSpecs(X509* ca_cert, ::Cert::x509::ExtSpecifications extra_extensions);

#pragma mark - certificate output functions

    std::string asPEMString();
    
    void writeToFilePEM(boost::filesystem::path  filePath);
    
    void print(boost::filesystem::path outputPath);

    void print();
    
    std::string printToString();

};

} // namespace Cert

#endif
