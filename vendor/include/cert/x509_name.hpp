#ifndef cert_x509_name_include_hpp
#define cert_x509_name_include_hpp

namespace Cert {
namespace x509 {

#pragma mark - name functions

    std::string Name_AsOneLine(X509_NAME* name);

    std::string Name_AsMultiLine(X509_NAME* name);

    NameSpecification  Name_getSpec(X509_NAME* name);

    X509_NAME*  Name_fromSpec(NameSpecification entries);

    void Name_AddEntryByNID(X509_NAME* name, int nid, std::string value);


} // namespace x509
} //namespace Cert

#endif
