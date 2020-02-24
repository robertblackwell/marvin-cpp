
#ifndef cert_x509_cert_include_hpp
#define cert_x509_cert_include_hpp
namespace Cert {
    class Authority;
}

namespace Cert {
namespace x509 {
    /**
    * @ingroup x509
    */
    #pragma mark - certificate read/write
    /**
    * Cert_ReadFromFile
    *
    * @discussion Read an X509* certificate from a file in PEM format
    * 
     * @param fileName as a c++ string
    *
    * @todo make fileName a boost::filesystem::path
    *
    * @returns X509* the caller is responsible for calling X509_free() when done
    * 
     * @throws exception on failure
    */
    X509* Cert_ReadFromFile(std::string fileName);

    X509* Cert_FromPEMString(std::string pem);
    
    /**
    * Writes a certificate in PEM format to a file
    *
    * @param cert  X509* certificate to write
    *
    * @param fileName  c++ string with file name/path to write to
    *
    * @throws if any error occurs
    */
    void Cert_WriteToFile(X509* cert, std::string fileName);
#pragma mark - cert verify
    /**
    * Verify a certificate against the provide CertificateAuthority
    * @param cert X509* - the certificate to verify
    * @param certAuth CertificateAuthority - to verify agains
    * @return bool
    */
    bool Cert_Verify(X509* cert, Cert::Authority& certAuth);
    /**
    * Verify a certificate against an existing certificate bundle file
    * @param cert X509* - the certificate to verify
    * @param cert_bundle_path - path to a file containing the certificate bundle to verify against
    * @return bool
    */
    bool Cert_Verify(X509* cert, std::string cert_bundle_path);
    /**
    * Verify a certificate against an existing X509_STORE* object
    * @param cert X509* - the certificate to verify
    * @param store X509_STORE - a X509 cert store already primed with the certificate bundle to verify against
    * @return bool
    */
    bool Cert_Verify(X509* cert, X509_STORE* store);

#pragma mark - certificate subject_name getters/setters
    /**
     * @brief     Get a certificates subject_name as a C++ datastructure,
     *
     * @param      cert  The cert, X509*
     *
     * @return     Cert::x509::NameSpecification. This return value can be used to
     *              set a certificates subject_name field
     * @throws an exception on any failure
     */
    ::Cert::x509::NameSpecification Cert_GetSubjectNameAsSpec(X509* cert);
    
    /**
     * @brief      Set the value of a certificates subject_name
     *              using a Cert::x509::NameSpecification ass the definition
     *              of what the name will contain. 
     *
     * @param      cert           X509*
     * 
     * @param[in]  subjectValues  The subject name component values
     * 
     * @throws an exception on any failure
     */
    void Cert_SetSubjectName(X509* cert, ::Cert::x509::NameSpecification subjectValues);

    std::string Cert_GetIssuerNameAsOneLine(X509* cert);
    std::string Cert_GetIssuerNameAsMultiLine(X509* cert);


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
    ::Cert::x509::NameSpecification Cert_GetIssuerNameAsSpec(X509* cert);

    /**
     * @brief      Set the value of a certificates issuer_name
     *              using a Cert::x509::NameSpecification ass the definition
     *              of what the name will contain. 
     *
     * @param      cert           The cert X509*
     * @param[in]  spec         The component values of the subject_name to be built
     * @throws an exception on any failure
     */
    void Cert_SetIssuerName(X509* cert, ::Cert::x509::NameSpecification spec);

    std::string Cert_GetSubjectNameAsOneLine(X509* cert);
    std::string Cert_GetSubjectNameAsMultiLine(X509* cert);

#pragma mark - certificate version getters/setters

    /**
    * Gets the version of a certificate
     * 
     * @param cert an X509* certificate
     * 
     * @return Cert::x509::Verions  - this is a by value return
     * 
     * @throws an exception on any failure
    */
    ::Cert::x509::Version Cert_GetVersion(X509* cert);

    /**
     * Sets the value of the version in a certificate.
     * 
     * @param cert an X509* certificate
     * 
     * @param version Cert::x509::Version
     *
     * This works   Cert_SetVersion(cert2, Cert_GetVersioine(cert1))
     *
     * @throws an exception on any failure
    */
    void Cert_SetVersion(X509* cert, ::Cert::x509::Version version);

#pragma mark - certificate serial number getters/setters

    /**
     * Sets the value of the serial number in a certificate.
     * @param cert an X509* certificate
     * 
     * @param serial an Cert::x509::Serial - 
     *
     * @throws an exception on any failure
    */
    void Cert_SetSerialNumber(X509* cert, ::Cert::x509::SerialNumber serial);

#pragma mark - certificate public key getters/setters

    EVP_PKEY* Cert_GetPublicKey(X509* cert);

    void Cert_SetPublicKey(X509* cert, EVP_PKEY* pubkey);

#pragma mark - certificate public key getters/setters

    void Cert_SetPublicKey(X509* cert, EVP_PKEY* pubkey);

#pragma mark - certificate not before and not after getters/setters

    void Cert_SetNotBefore(X509* cert, int offset_from_now_secs);

    void Cert_SetNotAfter(X509* cert, int offset_from_now_secs);

#pragma mark - extension functions
    /**
    * @brief    Gets a set of subject alternative names from a certificate's subject_alt_names extensions
    *
    * @param    cert    X509*
    *
    * @return a std::string
    */
    std::string Cert_GetSubjectAlternativeNamesAsString(X509*);
    /**
    * @brief    Gets a set of DNS names from a certificate's subject_alt_names extensions
    *
    * @param    cert    X509*
    *
    * @return AlternativeDNSNameSet
    */
    AlternativeDNSNameSet Cert_GetSubjectAlternativeDNSNames(X509* cert);

    /**
     * @brief      This function gets all the extensions from a certificate and returns
     *              in a C++ data structure that is ONLY useful for display. The returned
     *              value CANNOT be used to set the extensions of the same or a different
     *              certificate. 
     *
     * @param      cert  The cert X509*
     *
     * @return     extensions values as Cert::x509::ExtDescriptions 
     */
    ::Cert::x509::ExtDescriptions Cert_extensionsAsDescription(X509* cert);

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
    void Cert_AddExtensionsFromSpecs(X509* ca_cert, X509* cert, ::Cert::x509::ExtSpecifications extra_extensions);

#pragma mark - certificate output functions

    std::string Cert_PEMString(X509* cert);
    
    void Cert_WriteToFilePEM(X509* cert, std::string filename);
    
    void Cert_Print(X509* cert, BIO* out_bio);

    void Cert_Print(X509* cert);
    
    std::string Cert_PrintToString(X509* cert);


#if 0
#pragma mark - PRIVATE certificate subject_name getters/setters

    /**
     * Gets the subjectname of a certificate
     * @parameter cert an X509* certificate
     * @returns X509_NAME* - this is an internal pointer do not attempt to free it
     */
    X509_NAME* Cert_GetSubjectName(X509* cert);

    /**
    * Sets the value of the subjectname in a certificate.
    * @parameter cert an X509* certificate
    * @parameter name an X509_NAME structure - the function copies the X509_NAME structure
    *               so the caller is responsible for any free'ing of the name if appropriate. This means that
    *               the following code works
    *
    *               Cert_SetSubjectName(cert2, Cert_GetSubjectName(cert1))
    *
    * @throws an exception on any failure
    */
    void Cert_SetSubjectName(X509* cert, X509_NAME* name);

#pragma mark - PRIVATE certificate issuer_name getters/setters

    /**
    * Gets the issuername of a certificate
    * @parameter cert an X509* certificate
    * @returns X509_NAME* - this is an internal pointer do not attempt to free it
    */
    X509_NAME* Cert_GetIssuerName(X509* cert);

    /**
    * Sets the value of the issuername in a certificate.
    * @parameter cert an X509* certificate
    * @parameter name an X509_NAME structure - the function copies the X509_NAME structure
    *               so the caller is responsible for any free'iing of the name. This means that
    *               the following code works
    *
    *               Cert_SetIssuerName(cert2, Cert_GetIssuerName(cert1))
    *
    * @throws an exception on any failure
    */
    void Cert_SetIssuerName(X509* cert, X509_NAME* name);

#pragma mark - PRIVATE certificate serial number getters/setters

    /**
    * Gets the serial number of a certificate
    * @parameter cert an X509* certificate
    * @returns ASN1_INTEGER* - this is an internal pointer do not attempt to free it
    */
    ASN1_INTEGER* Cert_GetSerialNumber(X509* cert);

#pragma mark - PRIVATE certificate not before and not after getters/setters

    ASN1_TIME* Cert_GetNotBefore(X509* cert);

    void Cert_SetNotBefore(X509* cert, ASN1_TIME* tm);

    ASN1_TIME* Cert_GetNotAfter(X509* cert);

    void Cert_SetNotAfter(X509* cert, ASN1_TIME* tm);

#pragma mark - PRIVATE certificate extension functions

    STACK_OF(X509_EXTENSION)* Cert_GetExtensions(X509* cert);

    /**
    * Add an extensions to a certificate. The function copies the
    * extensions so that the caller is responsible for the ext pointer
    * after the call.
    *
    * @throws an exception if soomething goes wrong
    */
    void Cert_AddExtension(X509* cert, X509_EXTENSION* ext);

    X509_EXTENSION* Cert_GetSubjectAltName(X509* cert);
#endif
} // namespace x509
} //namespace Cert
#endif
/** @} */
