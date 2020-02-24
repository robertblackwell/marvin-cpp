#ifndef cert_x509_ext_include_hpp
#define cert_x509_ext_include_hpp

namespace Cert {
namespace x509 {

#pragma mark - extension methods

    typedef STACK_OF(X509_EXTENSION)* ExtensionStack;

    std::string Extension_ValueAsString(X509_EXTENSION* ext);

    ::Cert::x509::ExtDescriptions ExtensionStack_asDescriptions(STACK_OF(X509_EXTENSION)* stack);

    void ExtensionStack_AddByNID(STACK_OF(X509_EXTENSION) *stack, ::Cert::x509::ExtNid extid, std::string value);

    X509_EXTENSION* Extension_create(X509* cacert, X509* cert, ::Cert::x509::ExtNid nid, std::string specification);

#pragma mark - extension functions
    
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


} // namespace x509
} //namespace Cert

#endif
