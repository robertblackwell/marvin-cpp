#ifndef cert_x509_cert_impl_include_hpp
#define cert_x509_cert_impl_include_hpp
namespace Cert {
    class Authority;
}

namespace Cert {
namespace x509 {

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
    
} // namespace x509
} //namespace Cert
#endif
/**
* @}
*/
