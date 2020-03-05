
// dummy .cpp just to force cmake to see this lib as a target

// #include <stdio.h>
// #include <boost/filesystem.hpp>
// #include <boost/format.hpp>
// #include <iostream>
// #include <map>
// #include <sstream>

// #include <cert/cert.hpp>
// #include <cert/cert_chain.hpp>
// #include <cert/cert_store.hpp>

#include "test_fixture.hpp"

using namespace boost::filesystem;
using namespace Cert;
using namespace Cert::x509;
using namespace Cert::Helpers;
using namespace Cert::Store;
// changed in test_x509
static std::string without_header = R"HDR(
##
## This file contains roots certificates from Mozilla
##
## EXCEPT THAT the certificates for
##
## %1%
##
## has been removed
## diff against the one called with.pem to find out what is missng
##
)HDR";
static std::string with_header    = R"HDR(
##
## This file contains roots certificates from Mozilla
##
)HDR";


/**
* @brief A class that is a "test fixture" in the sense of Catch2 - a class that is derived from
* to make a TEST_CASE_METHOD. This one is common across all tests and really only provides location information
* about data in the "fixtures" or "test data" directory under ....tests.
* NOTE - it tests for the existence of the resource files associated with testing
* and hence should not be instanciated unless all resources have previously been
* put in place, with for example, test_data_init
*
*/
TestFixture::TestFixture() {
    m_ca_dir_basename  = "CA";
    m_this_file_path   = boost::filesystem::canonical(__FILE__);
    m_this_dir_path    = m_this_file_path.parent_path();
    m_test_dir_path    = m_this_file_path.parent_path();
    m_project_dir_path = m_test_dir_path.parent_path();
    m_fixture_dir_path = m_test_dir_path / "fixture";

    m_hosts_for_handshake = {
        "bankofamerica.com",
        "paypal.com",
        "www.google.com",
        "www.httpsnow.org",
        "yahoo.com",
        "www.wellsfargo.com",
        "www.digicert.com",
        "badssl.com",
        "www.godaddy.com"};

    m_host_for_wwo_test    = "bankofamerica.com";  // has multiple certs in its chain
    m_host_for_bundle_test = "badssl.com";
    m_host_for_forge_test  = "badssl.com";
}
// std::string             m_ca_dir_basename;
// boost::filesystem::path m_this_file_path;
// boost::filesystem::path m_this_dir_path;
// boost::filesystem::path m_test_dir_path;
// boost::filesystem::path m_project_dir_path;
// boost::filesystem::path m_fixture_dir_path;

// std::vector<std::string> m_hosts_for_handshake;
// Cert::Store::StoreSPtr   m_store_sptr;
// Cert::Store::LocatorSPtr m_locator_sptr;
// Cert::AuthoritySPtr      m_authority_sptr;
// std::string              m_host_for_wwo_test;
// std::string              m_host_for_bundle_test;
// std::string              m_host_for_forge_test;

path TestFixture::projectDirPath() {
    return m_project_dir_path;
}
path TestFixture::testsDirPath() {
    return (m_test_dir_path);
}
path TestFixture::preExistingTestDataDir() {
    return (testsDirPath() / "preexisting_test_data");
}
path TestFixture::preExistingTestFilePath(std::string filename) {
    return (testsDirPath() / "preexisting_test_data" / filename);
}

path TestFixture::TestFixture::preExistingCaConfigFilePath() {
    return preExistingTestFilePath("ca_config.json");
}

path TestFixture::preExistingHostADirPath() {
    return preExistingTestFilePath("host_a");
}

path TestFixture::preExistingHostARealCertFilePath() {
    return (preExistingHostADirPath() / "real_certificate.pem");
}

path TestFixture::preExistingHostBDirPath() {
    return preExistingTestFilePath("host_b");
}

path TestFixture::preExistingHostBRealCertFilePath() {
    return (preExistingHostBDirPath() / "real_certificate.pem");
}

path TestFixture::fixtureDirPath() {
    return (testsDirPath() / "fixture");
}

path TestFixture::fixturesFilePath(std::string fileName) {
    return (fixtureDirPath() / fileName);
}
path TestFixture::nonDefaultRootCertificateBundleFilePath() {
//        return "/usr/local/ssl/cert.pem";
    return "/usr/local/etc/openssl@1.1/cert.pem";
}

path TestFixture::storeRootDirPath() {
    return (fixtureDirPath() / m_ca_dir_basename);
}

path TestFixture::caPrivateDirPath() {
    return (storeRootDirPath() / "private");
}

path TestFixture::caSaveDirPath() {
    return caPrivateDirPath();
}

path TestFixture::caConfigFilePath() {
    return (fixtureDirPath() / "ca_config.json");
}

path TestFixture::certStoreRoot() {
    return storeRootDirPath();
}

path TestFixture::reqTestDirPath() {
    return (projectDirPath() / "req_test");
}

std::string TestFixture::caKeyPassword() {
    assert(false);
    return "blackwellapps";
}

path TestFixture::caPrivateKeyFilePath() {
    assert(false);
    return (caPrivateDirPath() / "cakey.pem");
}

path TestFixture::caCertPath() {
    assert(false);
    return (caPrivateDirPath() / "cacert.pem");
}

path TestFixture::hostsDirPath() {
    return (storeRootDirPath() / "hosts");
}

path TestFixture::hostCertificateDirPath(std::string host) {
    return (hostsDirPath() / host);
}

path TestFixture::realCertFilePathForHost(std::string host) {
    return (hostCertificateDirPath(host) / "real_certificate.pem");
}

path TestFixture::realCertChainFilePathForHost(std::string host) {
    return (hostCertificateDirPath(host) / "real_certificate_chain.pem");
}

path TestFixture::interceptCertForHostPath(std::string host) {
    return (hostCertificateDirPath(host) / "certificate.pem");
}

path TestFixture::rootCertificateBundleDirPath() {
    return (storeRootDirPath() / "root_certs");
}

path TestFixture::combinedRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "combined-cacert.pem");
}

path TestFixture::activeRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "active_roots.pem");
}

path TestFixture::osxCombinedRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "osx_ext.pem");
}

path TestFixture::mozCombinedRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "mozilla_ext.pem");
}

path TestFixture::osxRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "osx.pem");
}

path TestFixture::mozRootCertificateBundleFilePath() {
    return (rootCertificateBundleDirPath() / "mozilla.pem");
}

path TestFixture::ssltestCertificateFilePath() {
    return (storeRootDirPath() / "sites" / "ssltest" / "ssltest.crt.pem");
}

path TestFixture::withWithoutDirPath() {
    return (fixtureDirPath() / "with_without");
}

path TestFixture::withWithoutRootCertificateBundleFilePath(std::string with_without) {
    return (withWithoutDirPath() / (with_without + ".pem"));
}

std::vector<std::string> TestFixture::initialHosts() {
    return m_hosts_for_handshake;
}

std::string TestFixture::hostForWithWithoutTests() {
    return "bankofamerica.com";  // has multiple certs in its chain
}

std::string TestFixture::hostForBundleTests() {
    return "badssl.com";  // requires SNI done correctly
}

std::string TestFixture::hostForForgeTest() {
    return "badssl.com";  // requires SNI done correctly
}

std::string TestFixture::hostForBuildTest() {
    return "badssl.com";  // requires SNI done correctly
}

std::vector<std::string> TestFixture::badssl_SubDomians() {
    return {};
}

path TestFixture::testHostADirPath() {
    return (hostCertificateDirPath("host_a"));
}

path TestFixture::testHostBDirPath() {
    return (hostCertificateDirPath("host_b"));
}

/**
* This function builds or refreshes the tests/fixtures directory in preparartion for
* running the test suite. Values used in the building of fixtures are taken from
* a file ${project_dir}/tests/test_config.json
*
* In order to test this function it is possible to have this function build the "fixtures"
* directory in a different place with a diffeerent name.
*/
void TestFixture::setup() {
    m_ca_dir_basename  = "CA";
    m_this_file_path   = boost::filesystem::canonical(__FILE__);
    m_this_dir_path    = m_this_file_path.parent_path();
    m_test_dir_path    = m_this_file_path.parent_path();
    m_project_dir_path = m_test_dir_path.parent_path();
    m_fixture_dir_path = m_test_dir_path / "fixture";

    m_hosts_for_handshake = {
        "bankofamerica.com",
        "paypal.com",
        "www.google.com",
        "www.httpsnow.org",
        "yahoo.com",
        "www.wellsfargo.com",
        "www.digicert.com",
        "badssl.com",
        "www.godaddy.com"};

    m_host_for_wwo_test    = "bankofamerica.com";  // has multiple certs in its chain
    m_host_for_bundle_test = "badssl.com";
    m_host_for_forge_test  = "badssl.com";

    /// clean out the fixtureCert Store pointed at by the test fixture
    /// are we building the "fixture" directory in a dummy place for testing this
    /// function. Or are we building it in the real locations
    /// use the default root for the fixture
    path fixture_root = m_fixture_dir_path;  // h.fixture_path();
    path store_root   = storeRootDirPath();  //path(h.certStoreRoot());
    path caSaveDir    = caPrivateDirPath();
    /// now clean out the "fixture" directory (or its proxy) in prep
    /// for building/copying in all the test data
    remove_all(fixtureDirPath());
    create_directories(fixtureDirPath());

    /// are we building the fixture directory with a new CA whose details come from
    /// preexisting_test_data/ca_spec.json. Or are we copying in a CA already built.
    ///
    /// The default and correct is new CA - the option will be removed before release
    ///
    if (true) {
        /**
            * Initialize the fixtures dir with a store that has a new CA derived from
            * config data in tests/preexisting_test_data/test_config.json file
            */
        boost::filesystem::path config_from = preExistingCaConfigFilePath();
        boost::filesystem::path config_to   = caConfigFilePath();
        copy_file(config_from, config_to);

        m_store_sptr     = Cert::Store::Store::makeWithCA(storeRootDirPath(), config_to);
        m_locator_sptr   = m_store_sptr->getLocator();
        m_authority_sptr = m_store_sptr->getAuthority();
    } else {
#if 0
            /**
            * Initialize the fixtures dir with the already existing CA data .. only
            * for when testing on my private development machine
            */
            m_store_sptr = Cert::Store::Store::makeEmpty(store_root);
            m_locator_sptr = m_store_sptr->m_locator_sptr;
            // now copy the ca into the test fixture cet store
            copy_file(original_ca_dir_path / "private" / "cakey.pem", m_locator_sptr->ca_key_pem_path);
            copy_file(original_ca_dir_path / "private" / "cacert.pem", m_locator_sptr->ca_cert_pem_file_path);
            copy_file(original_ca_dir_path / "private" / "ca.p12", m_locator_sptr->ca_pk12_file_path);
            copy_file(original_ca_dir_path / "private" / "caroot.cnf", m_locator_sptr->ca_cnf_file_path);
            copy_file(original_ca_dir_path / "config.json", m_locator_sptr->config_file_path );
#endif
    }
    /// now set up the various bundles of root certificates
    m_store_sptr->rootCertsFromMozilla();
    m_store_sptr->rootCertsFromKeychain();

    m_store_sptr->rootCertsMozillaActive();
    /// copy over predefined test data
    boost::filesystem::create_directories(hostCertificateDirPath("host_a"));
    copy_file(
        preExistingHostARealCertFilePath(),
        realCertFilePathForHost("host_a"));
    boost::filesystem::create_directories(hostCertificateDirPath("host_b"));
    copy_file(
        preExistingHostBRealCertFilePath(),
        realCertFilePathForHost("host_b"));
    ///
    /// download certificates and certificate chains for a selection of hosts
    ///
    std::vector<std::string> hosts = m_hosts_for_handshake;
    for (const std::string& h : hosts) {
        Host::create(*m_store_sptr, h);
    }

    ///  now prepares the with_without_with and with_without_without bundles for forcing a verification failure
    //   when handshaking with the host specified by helper.withWithoutHost()
    create_directories(withWithoutDirPath());

    ///
    /// slightly reformat and then write the mozilla bundle to the withwithout/with.pem - file
    ///
    Cert::Chain moz_chain(m_locator_sptr->mozilla_root_certs);
    moz_chain.writePEM(withWithoutRootCertificateBundleFilePath("with"));
    ///
    /// Now create the withwithout/without.pem bundle
    /// by filtering out of the "with" bundle the root certificates from
    /// the mozilla bundle that is the issuer of the final certificate
    /// in the certificate chain of the helper.withWithoutHost().
    /// Thus the helper.withWithoutHost() should FAIL to verify
    /// agsin the without.pem bundle
    ///
    std::string wwohost = m_host_for_wwo_test;
    Cert::Chain hostchain(m_locator_sptr->hostRealCertificateChainPath(wwohost));
    auto        c1               = hostchain.toPEMString();
    std::string issuer_to_remove = hostchain.lastIssuer();
    std::string wwoh_header_text = str(boost::format(without_header) % issuer_to_remove);
    // NOTE: this is a hack. Based on knowledge that the root cert for bankofamerica
    // is issued by Entrust
    auto new_bundle = moz_chain.removeAllSubjectsMatching(".*Entrust.*");
    new_bundle.writeAnnotated(withWithoutRootCertificateBundleFilePath("without"), wwoh_header_text);
    /**
        * At this point - we have all directories created, a CA and various bundles of root
        * certificates and we have copied over all predefined test data. This is enough
        * to let us create a fixture object. Remember the TestFixture class checks
        * for the existence of various files and hence this creation is in part of verification
        * that we have done everything right
        */
    assert(filesystem::is_regular(mozRootCertificateBundleFilePath()));
    assert(filesystem::is_regular(osxRootCertificateBundleFilePath()));
    assert(filesystem::is_regular(osxCombinedRootCertificateBundleFilePath()));
    assert(filesystem::is_regular(withWithoutRootCertificateBundleFilePath("with")));
    assert(filesystem::is_regular(withWithoutRootCertificateBundleFilePath("without")));
    assert(filesystem::is_directory(testHostADirPath()));
    assert(filesystem::is_directory(testHostBDirPath()));

    std::cout << "hello" << std::endl;
}
void TestFixture::loadExisting() {
    
    m_store_sptr     = Cert::Store::Store::load(storeRootDirPath());
    m_locator_sptr   = m_store_sptr->getLocator();
    m_authority_sptr = m_store_sptr->getAuthority();
}
