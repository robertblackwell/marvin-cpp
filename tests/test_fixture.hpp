#ifndef test_fixtures_2_hpp
#define test_fixtures_2_hpp

#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <map>
#include <sstream>

#include <cert/cert.hpp>
#include <cert/cert_chain.hpp>
#include <cert/cert_store.hpp>

using namespace boost::filesystem;
/**
* @brief A class that is a "test fixture" in the sense of Catch2 - a class that is derived from
* to make a TEST_CASE_METHOD. This one is common across all tests and really only provides location information
* about data in the "fixtures" or "test data" directory under ....tests.
* NOTE - it tests for the existence of the resource files associated with testing
* and hence should not be instanciated unless all resources have previously been
* put in place, with for example, test_data_init
*
*/
class TestFixture {
    public:
    /**
    * create a fixture object and initialize dir/file path names so that the instance can successfully
    * access assets that have previously been built.
    *
    * If new/clean assets are to be build call setup() after instanciation
    */
    TestFixture();
    /**
    * This function builds or refreshes the tests/fixtures directory in preparartion for
    * running the test suite. Values used in the building of fixtures are taken from
    * a file ${project_dir}/tests/test_config.json
    *
    * In order to test this function it is possible to have this function build the "fixtures"
    * directory in a different place with a diffeerent name.
    */
    void setup();
    void loadExisting();

    std::string             m_ca_dir_basename;
    boost::filesystem::path m_this_file_path;
    boost::filesystem::path m_this_dir_path;
    boost::filesystem::path m_test_dir_path;
    boost::filesystem::path m_project_dir_path;
    boost::filesystem::path m_fixture_dir_path;

    std::vector<std::string> m_hosts_for_handshake;
    Cert::Store::StoreSPtr   m_store_sptr;
    Cert::Store::LocatorSPtr m_locator_sptr;
    Cert::AuthoritySPtr      m_authority_sptr;
    std::string              m_host_for_wwo_test;
    std::string              m_host_for_bundle_test;
    std::string              m_host_for_forge_test;

    path projectDirPath();
    path testsDirPath();
    path preExistingTestDataDir();
    path preExistingTestFilePath(std::string filename);
    path preExistingCaConfigFilePath();
    path preExistingHostADirPath();
    path preExistingHostARealCertFilePath();
    path preExistingHostBDirPath();
    path preExistingHostBRealCertFilePath();
    path fixtureDirPath();
    path fixturesFilePath(std::string fileName);
    path nonDefaultRootCertificateBundleFilePath();
    path storeRootDirPath();
    path caPrivateDirPath();
    path caSaveDirPath();
    path caConfigFilePath();
    path certStoreRoot();
    path reqTestDirPath();
    std::string caKeyPassword();
    path caPrivateKeyFilePath();
    path caCertPath();
    path hostsDirPath();
    path hostCertificateDirPath(std::string host);
    path realCertFilePathForHost(std::string host);
    path realCertChainFilePathForHost(std::string host);
    path interceptCertForHostPath(std::string host);
    path rootCertificateBundleDirPath();
    path combinedRootCertificateBundleFilePath();
    path activeRootCertificateBundleFilePath();
    path osxCombinedRootCertificateBundleFilePath();
    path mozCombinedRootCertificateBundleFilePath();
    path osxRootCertificateBundleFilePath();
    path mozRootCertificateBundleFilePath();
    path ssltestCertificateFilePath();
    path withWithoutDirPath();
    path withWithoutRootCertificateBundleFilePath(std::string with_without);
    std::vector<std::string> initialHosts();
    std::string hostForWithWithoutTests();
    std::string hostForBundleTests();
    std::string hostForForgeTest();
    std::string hostForBuildTest();
    std::vector<std::string> badssl_SubDomians();
    path testHostADirPath();
    path testHostBDirPath();
};
#endif
