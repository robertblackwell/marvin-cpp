#ifndef cert_x509_pkey_include_hpp
#define cert_x509_pkey_include_hpp

/*!
* @brief Cert::x509::ExtNid  List of identifiers for the NID/OID of the extensions we are interested in
*
* This enumeration is provided to enforce some level of type checking.
*
* See x509/x509_nid.hpp for functions to retreive details of these NID values
*
* @note did not use an enum class as might want these to be visible to objective c
*/
namespace Cert {
namespace x509 {

#pragma mark - pkeys, EVP_PKEY*

    EVP_PKEY* Rsa_Generate();

    EVP_PKEY* PKey_ReadPrivateKeyFrom(std::string fileName, std::string password);

    EVP_PKEY* PKey_ReadPrivateKeyFrom(std::string fileName);

    void PKey_WritePrivateKey(EVP_PKEY* pkey, std::string filename, std::string password);

    std::string
    PKey_PublicKeyAsPEMString(EVP_PKEY* pkey);
    
    std::string PKeyPrivate_AsPEMString(EVP_PKEY* pkey);
    EVP_PKEY* PKeyPrivate_FromPEMString(std::string pem);
    std::string PKeyPrivatePEM_FromFile(std::string filename);
    void PKeyPrivatePEM_ToFile(std::string pem, std::string filename, std::string password);
} // namespace x509
} //namespace Cert

#endif /* x509_pkey_hpp */
