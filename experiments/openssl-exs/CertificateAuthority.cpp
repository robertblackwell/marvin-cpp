//
//  CertificateAuthority.cpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 10/25/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#include "CertificateAuthority.hpp"
CertificateAuthority::CertificateAuthority(std::string caDirectory) : _caDirectory(caDirectory)
{
    _caCertFileName = _caDirectory + "/cacert.pem";
    _caPKeyFileName = _caDirectory + "/cakey.pem";
    _serialNumberFileName = _caDirectory + "/serial";
    std::string  caKeyPassword;
    std::string passwordFileName = _caDirectory + "/password";
    std::ifstream passwordFile;
    passwordFile.open(passwordFileName);
    passwordFile >> caKeyPassword;
    passwordFile.close();
    std::string _caKeyPassword;
    _caCert = x509Cert_ReadFromFile(_caCertFileName);
    _caPKey = x509PKey_ReadPrivateKeyFrom(_caPKeyFileName, caKeyPassword);
    std::ifstream serialNumberFile;
    serialNumberFile.open(_serialNumberFileName);
    serialNumberFile >> nextSerialNumber;
    serialNumberFile.close();

}
CertificateAuthority::~CertificateAuthority()
{
    X509_free(_caCert);
    EVP_PKEY_free(_caPKey);
}
X509* CertificateAuthority::getCACert()
{
    return _caCert;
}
EVP_PKEY* CertificateAuthority::getCAPKey()
{
    return _caPKey;
}
int CertificateAuthority::getNextSerialNumber()
{
    return nextSerialNumber;
}
