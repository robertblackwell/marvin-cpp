#ifndef certlib_cert_authority_include_hpp
#define certlib_cert_authority_include_hpp
#include <vector>
#include <boost/format.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <json/json.hpp>

#include "cert_helpers.hpp"

using namespace Cert;
using namespace boost;
namespace Cert {

    filesystem::path caCertPemFilePath(filesystem::path caDirPath);
    filesystem::path caPrivateKeyPemPath(filesystem::path caDirPath);
    filesystem::path caPk12FilePath(filesystem::path caDirPath);
    filesystem::path caSelfSignRootCnfPath (filesystem::path caDirPath);
    filesystem::path caKeyPasswordFilePath(filesystem::path caDirPath);
    filesystem::path caSerialNumberFilePath(filesystem::path caDirPath);
    filesystem::path caCnfFilePath (filesystem::path caDirPath);


/**
 * @brief Namespace to wrap functions that are helpers for Cert::Authority.
 *
 */
namespace AuthHelpers {

///**
//* \brief Create a string containing openssl config details from which to create a
//* self signed certificate authority
//*
//* @param ca_name std::string - the common name of the CA
//* @param state std::string - the state for the CA
//* @param country std::string - the country for the CA
//* @param email std::string - the email address for the CA
//* @param organization std::string - the org name for the CA
//* @param cakey_pem Store::Path - path for CA key output
//* @return std::string
//*/
//static std::string ca_cnf_string(
//    std::string ca_name,
//    std::string state,
//    std::string country,
//    std::string email,
//    std::string organization,
//    boost::filesystem::path cakey_pem
//);
///**
//* \brief Create a shell command string to make a CA key
//* an existing config file
//*
//* @param cakey_pem Store::Path - path to write the key to
//* @param passout std::string - the pass phrase used to secure the key when it is outputted
//* @return std::string
//*/
//static std::string ca_key_cmd(boost::filesystem::path cakey_pem, std::string passout);
///**
//* \brief Create a shell command string to make a self signed CA certificate using
//* an existing config file
//*
//* @param cakey_pem Store::Path - path to the CA key which must already exist
//* @param cacert_pem Store::Path - path for CA certificate
//* @param ca_self_signed_root_cnf Store::Path - path to config file to use for openssl x509 command
//* @return std::string
//*/
//static std::string ca_cert_cmd(
//    boost::filesystem::path cakey_pem,
//    boost::filesystem::path cacert_pem,
//    boost::filesystem::path ca_self_signed_root_cnf,
//    std::string passin,
//    std::string passout);
///**
//* \brief Create a shell command string to convert CA key and cert into pk12 format
//*
//* @param ca_name std::string - the comman name of the CA
//* @param cakey_pem Store::Path - path for CA private key
//* @param cacert_pem Store::Path - path for CA certificate
//* @param passin std::string - in password
//* @param passout std::string - out passowrd
//* @param ca_pk12 Store::Path - path for CA certificate + pkey in pk12 format
//* @return std::string
//*/
//static std::string ca_pk12_cmd(
//    std::string ca_name,
//    boost::filesystem::path cakey_pem,
//    boost::filesystem::path cacert_pem,
//    boost::filesystem::path ca_pk12,
//    std::string passin,
//    std::string passout
//);

/**
 * Warning: this function executes console commands using the
 * boost::process::system() lib function
 *
 * create a certificate authority in the provide Store. Specifically that means:
 *
 *   -   create an openssl cnf file with the relevant ca details and save it in the stores ca directory
 *
 *   -   put the password for the ca's private key in a file and save that in the store's ca directory
 *
 *   -   run an openssl command to create a new key in pem format and save in the stores'ca directory and protect that
 *       key with the password saved in earlier step
 *
 *   -   run an openssl command to generate a self signed certificate using the key geneated in the previous
 *       step and save that in the store's ca directory
 *
 *   -   run an openssl command to combine the newly created private key and certificate into a pkcs12
 *       file and put that in the store' ca directory
 *
 * @param store Store - the certificate store where the CA is to be created
 *
 * assumption : details of the CA have already been stored in the store config file
*/
void createCertAuthority(
    boost::filesystem::path caDirPath,
    boost::filesystem::path caJsonSpecificationFile
);

} // namespace AuthHelpers
} //namespace Cert
#endif
