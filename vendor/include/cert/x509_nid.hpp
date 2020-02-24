#ifndef cert_x509_nid_include_hpp
#define cert_x509_nid_include_hpp

namespace Cert {
namespace x509 {



#pragma mark - NidDescriptor struct

    /*!
     *  Cert::x509::NidDescriptor
     *  
     *  @brief contains details of a NID/ASN1 object id
     *
     *  A NID is an integer index into a list of OID's (Object IDs) that are in ASN1 format.
     *  within openssl all kinds of values are identified by
     *   -   an NID which is an integer index
     *   -   an OID which is a string something like 2.1.45.7.1209
     *   -   a hex string (which is a compressed form of an OID value)
     *   -   NIDs also have long and short string descriptions and the NID can be found from the short name
     *
     *   A number of such NID/OID are defined at compile time within the crypto library, 
     *   see the header files in openssl/crypto/objects.h
     *
     *   Additionals OID/NID can be added at run time using
     *   -   OBJ_create(), the hard work is done by:
     *   -   int a2d_ASN1_OBJECT(unsigned char *out, int olen, const char *buf, int num)
     *
     *  @note why the property valid ? - I wanted to be able to return an instance of a Cert::x509::NidDescriptor
     *  (using return by value not return by pointer) even in situations where the function failed.
     *  I want to signal that failure by setting valid=false
     *  and I wanted to write that code as
     *
     *  if( !(nid_descriptor = functioncall(...)) ) {
     *      process error condition
     *  }
     *  rather than writing
     *
     *  nid_descriptor = functioncall(....)
     *  if( ! nid_descriptor.valid) {
     *      process error condition
     *  }
     *
     *  Here is a very good discussion of the "safe boolean"
     *  http://www.artima.com/cppsource/safebool.html
     *
     *  I have opted for the "doube ! trick" - BUT note the following is illegal
     *
     *  if( nid_descriptor = functioncall(....) ){
     */
    struct NidDescriptor 
    {
        /// @brief true if the collection of this data was successful false otherwise
        bool        valid;
        
        /// @brief integer value of nid
        int         nid;
        /// @brief long name of this object/nid
        std::string long_name;
        
        /// @brief short name of this object/nid
        std::string short_name;
        
        /// @brief a text string giving the hex value of the corresponding object - see openssl/objects/objects.h and openssl/objects/obj_dat.h
        std::string oid_hex;
        
        /// @brief - the oid description of this object as a text string
        std::string oid_numeric;
        
        /*!
        *  beware the double !! trick see http://www.artima.com/cppsource/safebool.html
        */
        bool operator!();
    };


#pragma mark - nid

    /*!
     * Gets a descriptor for a nid value.
     *
     * Return : always returns by value a Cert::x509::NidDescriptor instance.
     *
     * Throws exception if nid is invalid - ie has no details to be found
     *
     * There should never be a failure if valid values of Cert::x509::ExtNid_Enum
     * are used
    */
    ::Cert::x509::NidDescriptor Nid_GetDescriptor(::Cert::x509::ExtNid nid);

    /*!
     * Gets a descriptor for an ASN1_OBJECT value.
     *
     * Return : always returns by value a Cert::x509::NidDescriptor instance.
     *
     * Throws exception if the object represents an unknown OID
     *
    */
    Cert::x509::NidDescriptor Nid_DescriptorFromObject(ASN1_OBJECT* obj);



} // namespace x509
} //namespace Cert

#endif
