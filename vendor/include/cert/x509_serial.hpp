#ifndef cert_x509_serial_include_h
#define cert_x509_serial_include_h


#pragma mark - name Serial number

namespace Cert {
namespace x509 {

    std::string Serial_AsString(ASN1_INTEGER* serial);
} // namespace x509
} //namespace Cert


#endif /* serial */
