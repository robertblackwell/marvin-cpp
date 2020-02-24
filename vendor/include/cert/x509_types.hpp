
#ifndef certlib_x509_type_include_h
#define certlib_x509_type_include_h

namespace Cert {
/**
* @brief This namespace wraps a set of types and functions that in turn wrap openssl functions and provide
* create, access and modify x509 certificates and their properties. Saves me from having to remember all the openssl/x509
* functions and rules.
*/
namespace x509 {

#pragma mark - enumeration types
/*!
* @brief Cert::x509::ExtNid  List of identifiers for the NID/OID of the extensions we are interested in
*
* This enumeration is provided to enforce some level of type checking.
*
* See x509/x509_nid.hpp for functions to retreive details of these NID values
*
* @note did not use an enum class as might want these to be visible to objective c
*/

    typedef int ExtNid;
    enum ExtNid_Enum
    {
        ExtNid_basicConstraints            = NID_basic_constraints,
        ExtNid_subjectAltName              = NID_subject_alt_name,
        ExtNid_subjectAlternativeName      = NID_subject_alt_name,
        ExtNid_issuerAlternativeName       = NID_issuer_alt_name,
        ExtNid_keyUsage                    = NID_key_usage,

        ExtNid_authorityKeyIdentifier      = NID_authority_key_identifier,
        ExtNid_subjectKeyIdentifier        = NID_subject_key_identifier,
        ExtNid_certificatePolicies         = NID_certificate_policies,
        ExtNid_policyMappings              = NID_policy_mappings,
        ExtNid_subjectDirectoryAttributes  = NID_subject_directory_attributes,
        ExtNid_nameConstraints             = NID_name_constraints,
        ExtNid_policyConstraints           = NID_policy_constraints,
        ExtNid_extendedKeyUsage            = NID_ext_key_usage,
        ExtNid_crlDistributionPoints       = NID_crl_distribution_points,
        ExtNid_inhibitAnyPolicy            = NID_inhibit_any_policy,
        ExtNid_freshestCrl                 = NID_freshest_crl,
        ExtNid_authorityInformationAccess  = NID_info_access,
        ExtNid_subjectInformationAccess    = NID_sinfo_access
    };

    typedef int NameNid;
    enum NameNid_Enum
    {
        NameNid_countryName = NID_countryName,
        NameNid_organizationName = NID_organizationName,
        NameNid_organizationalUnitName = NID_organizationalUnitName,
        NameNid_distinguishedName = NID_distinguishedName,
        NameNid_stateOrProvince = NID_stateOrProvinceName,
        NameNid_streetAddress = NID_streetAddress,
        NameNid_commonName = NID_commonName,

        NameNid_localityName = NID_localityName,
        NameNid_title = NID_title,
        NameNid_surname = NID_surname,
        NameNid_givenName = NID_givenName,
        NameNid_initials = NID_initials,
        NameNid_pseudonym = NID_pseudonym,
        NameNid_generationQualifier = NID_generationQualifier
    };

    typedef long Version;
    enum VersionEnum
    {
        Version_v1 = 0L,
        Version_v2 = 1L,
        Version_v3 = 2L
    };
    
#pragma mark - typedefs
    
    /**
     * convenience types to clarify meaning
     */
    typedef long  SerialNumber;
    typedef int   TimeOffset;
    typedef void* PublicKey;
    typedef void* PKeyPair;


    /**
    * A map of string specifications indexed by Cert::x509::NameNid, the value of this type is a specification for how to build
    * an X509_NAME object. The string are values and will appear in the final X509_NAME object in an ASN1 encoded form
    */
    typedef std::map<Cert::x509::NameNid, std::string> NameSpecification;

    /**
     * A map of string specifications indexed by Cert::x509::ExtNid values is a specification for how to build
    * a STACK_OF(X509_EXTENSION). The strings are sometimes values but sometimes NOT values but keywords that
    * the extension building code (in openssl) interprets and then included an entirely different value.
    * For example a substring of the certificate or ca_certificate public key
    *
    *  see, for example http://openssl.cs.utah.edu/docs/apps/x509v3_config.html#certificate_policies_
    *
    * NOTE: this type is used to create STACK_OF(X509_EXTENSION)
    */
    typedef std::map<Cert::x509::ExtNid, std::string> ExtSpecifications;

    /**
    * This typedef appears identical to the previous one - however the name differences are intended to stress
    * that Cert::x509::ExtensionsSpec are used in construction of an extension stack whereas this type (description)
    * is merely the values extracted from an existing extension stack and CANNOT be used
    * to make a copy of the source extension stack
    *
    * NOTE: extracted from an existing STACK_OF(X509_EXTENSION) and CANNOT be used to create a new STACK_OF(X509_EXTENSION)
    */
    typedef std::map<Cert::x509::ExtNid, std::string> ExtDescriptions;
    
    /**
    * This type is used to house a collection of strings that are the values DNS names in the subject_alt_names
    * extensions in a Certificate. Used this data structure to allow easy equality test
    */
    typedef boost::unordered::unordered_set<std::string> AlternativeDNSNameSet;
} // namespace x509
} //namespace Cert


#endif /* x509_types_hpp */
