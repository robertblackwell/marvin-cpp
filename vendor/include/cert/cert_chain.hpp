#ifndef certlib_oo_cert_chain_hpp
#define certlib_oo_cert_chain_hpp

#include "cert.hpp"

namespace Cert {

class Chain
{
public:
    Chain();
    ~Chain();
    Chain(std::vector<std::string>& vec);
    Chain(boost::filesystem::path pem);
    Chain(std::string pem);

    void
    push_back(std::string pem);

    std::string lastIssuer();
    
    Chain removeAllSubjectsMatching(std::string name_to_remove);

    Chain removeSubject(std::string subName);

    std::string toPEMString();

    void writePEM(boost::filesystem::path filePath);

    void writeAnnotated(boost::filesystem::path filePath, std::string header);

    void print(boost::filesystem::path filePath);

    void print();

    std::string printToString();
    
private:

    class Impl;
    typedef std::shared_ptr<Impl> ImplSPtr;
    
    ImplSPtr m_impl_sptr;
};
} //namespace Cert

#endif
