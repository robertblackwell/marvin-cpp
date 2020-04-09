#include <marvin/certificates/env_utils.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <marvin/boost_stuff.hpp>
#include <marvin/certificates/certificates.hpp>

namespace Marvin {
// cwd should be either the users home dir of
// the marvin++ project root dir
bool validWorkingDir()
{
    using namespace boost::filesystem;
    path pwd_path(boost::filesystem::current_path());
    auto pwd_base = basename(pwd_path);
    if (pwd_base != "marvin++") {
        auto h = getenv("HOME");
        if (!h) {
            return false;
        }
        path user_home = path(std::string(h));
        return (user_home == pwd_path);
    } 
    return true;
}
boost::optional<std::string> getEnv(std::string name)
{
    using namespace boost::filesystem;

    auto ev = getenv(name.c_str());
    if (!ev) {
        return boost::optional<std::string>{};
    }
    std::string evs(ev);
    return boost::optional<std::string>(evs);
}

boost::optional<boost::filesystem::path>getEnvMarvinHome()
{
    using namespace boost::filesystem;
    auto mh3 = getenv(Marvin::kMarvinEnvKey_MarvinHome);

    auto mh = getenv("MARVIN_HOME");
    if (!mh) {
        return boost::optional<path>{};
    }
    path mh_p(std::string(mh));
    return boost::optional<boost::filesystem::path>(mh);
}
boost::optional<boost::filesystem::path> getEnvUserHome()
{
    using namespace boost::filesystem;

    auto mh = getenv("HOME");
    if (!mh) {
        return boost::optional<path>{};
    }
    path mh_p(std::string(mh));
    return boost::optional<boost::filesystem::path>(mh);
}

bool validEnvVariables()
{
    using namespace boost::filesystem;

    auto mh = getEnvMarvinHome();
    if (!mh) {
        return false;
    }
    auto uh = getEnvUserHome();
    if (!uh) {
        return false;
    }
    path uh_p = uh.get();
    path mh_p = mh.get();
    if (uh_p == mh_p) {
        return true;
    }
    if (mh_p == (uh_p /"Projects/marvin++")) {
        return true;
    }
    return false;

}

void setEnvMarvinHome(boost::filesystem::path h)
{
    std::string h_s = h.string();
    std::string s1 = h_s;
    const char* cstr1 = s1.c_str();
    char* cstr = const_cast<char*>(cstr1);
    setenv("MARVIN_HOME", cstr, 1);
    auto xx = getenv("MARVIN_HOME");
}

void setMarvinHomeToUserHome()
{
    char* hh = getenv("HOME");
    std::string home(hh);
    std::string s1 = home;
    const char* cstr1 = s1.c_str();
    char* cstr = const_cast<char*>(cstr1);
    setenv("MARVIN_HOME", cstr, 1);
    auto xx = getenv("MARVIN_HOME");
}
void setMarvinHomeToProjectHome()
{
    using namespace boost::filesystem;

    char* hh = getenv("HOME");
    std::string home(hh);
    path homepath(home);
    std::string s1 = (homepath / "Projects/marvin++").string();
    const char* cstr1 = s1.c_str();
    char* cstr = const_cast<char*>(cstr1);
    setenv("MARVIN_HOME", cstr, 1);
    auto xx = getenv("MARVIN_HOME");
}
} // namespace
