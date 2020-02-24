
#ifndef certlib_oo_cert_identity_include_h
#define certlib_oo_cert_identity_include_h
#include "x509.hpp"
#include "cert_certificate.hpp"
#include "cert_evp_pkey.hpp"

namespace Cert {
    
class Identity;
    /**
    * \brief This class carries a private key and related certificate and is used to provide an identity for
    * a server or certificate authority
    * This class helps manage lifetimes of pkeys and certificates. Think of it as a home grown std::unique_ptr<>.
    * Copyable and copy assignable but not Movable.
    * An Identity can be treated as a value, passed by value and assigned
    */
    class  Identity {
    public:
        // cannot create an 'empty' identity
        Identity() = delete;
// Including the next two lines causes some compile errors in Builder related to
// assignment from a function result
//        Identity(Identity&& other) = delete;
//        Identity& operator=(Identity&& other) = delete;

        Identity(const Identity& other) = default;
        Identity& operator=(const Identity&) = default;

        ~Identity();
        Identity(X509* certificate, EVP_PKEY* keyptr);
        Identity(const Cert::Certificate& certificate, const Cert::EvpPKey& keyPair);
        
        
        Cert::Certificate getCertificate();
        std::string getCertificatePEM();
        Cert::EvpPKey getEvpKey();
        std::string getEvpKeyPEM();

        // The pointer returned by this method is owned by the class instance. Do not try
        // and free it.
        X509* getX509();
        
        // The pointer returned by this method is owned by the class instance. Do not try
        // and free it.
        EVP_PKEY* getEVP_PKEY();
    private:
        X509*       m_cert;
        EVP_PKEY*   m_private_key;
    };

} //namespace Cert


#endif /* x509_identity_hpp */
