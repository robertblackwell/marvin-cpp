#ifndef cert_x509_conf_include_hpp
#define cert_x509_conf_include_hpp

namespace Cert {
namespace x509 {

#pragma mark - CONF class and functions

    /**
    * \brief a wrapper class around an openssl CONF* - which holds the contents of an
    * openssl .cnf file.
    *
    * Member functions allow access to variable values within the loaded inage of a .cnf file
    *
    */
    class Conf
    {
        public :
            // the ONLY way to make one is to load from a file. Cannnot copy or move one of these
            Conf(std::string filename);
            ~Conf();
        
            Conf() = delete;
            Conf(Conf&& other) = delete;
            Conf& operator=(Conf& other) = delete;
            Conf(const Conf&) = delete;
            Conf& operator=(Conf const&) = delete;

            int         getGlobalNumber(std::string key);
            std::string getGlobalString(std::string key);
            std::string getSectionString(std::string section, std::string key);
            std::map<std::string, std::string> getAllSectionStringValues(std::string section);


        private:
            CONF* _conf;
    };

} // namespace x509
} //namespace Cert
#endif
