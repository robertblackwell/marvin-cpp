#ifndef cert_authority_include_hpp
#define cert_authority_include_hpp

#include <boost/filesystem.hpp>
#include "x509.hpp"

namespace Cert {

class Authority;

typedef std::shared_ptr<Authority> AuthoritySPtr;
/**
* \brief Encapsulates a certificate authority by holding a private key, a certificate
 * and keeping track of serial numbers. The CA details can be persisted between "sessions"
 * Via the methods save() and load()
*/
class Authority
{

public:
        static AuthoritySPtr create(boost::filesystem::path caSaveDir, boost::filesystem::path jsonConfigFilePath);
        /**
         * Load a previously created CA from a directory
         * @param caDir
         * @return
         */
        static AuthoritySPtr load(boost::filesystem::path caDir);

        Authority(boost::filesystem::path ca_dir_path);
        Authority() = delete;

        ~Authority();
        /**
        * \brief Get an X509* referencing the Certificate Authorities certificate.
        * The object referenced is owned by this instance of CertificateAuthority
        * and should not be freed by the caller
        */
        X509* getCACert();
        /**
        * \brief Get an EVP_PKEY* referencing the Certificate Authorities private key.
        * The object referenced is owned by this instance of CertificateAuthority
        * and should not be freed by the caller
        */
        EVP_PKEY* getCAPKey();
    
        int getNextSerialNumber();

        /**
        * \brief Returns the full path to a file where the CA's certificate has been saved
        */
        std::string getCaCertPath();

    private:
        std::string m_ca_dir_path;
        std::string m_ca_cert_file_path;
        std::string m_ca_pkey_file_path;
        std::string m_serial_number_file_path;
        std::string m_password_file_path;
    
        X509*       m_ca_cert;
        EVP_PKEY*   m_ca_pkey;
        std::string m_ca_key_password;
        int         m_next_serial_number;
        
    };
} //namespace Cert

#endif
