#ifndef marvin_env_utils_hpp
#define marvin_env_utils_hpp

#include <string>
#include <unistd.h>
#include <marvin/boost_stuff.hpp>

namespace Marvin {
// cwd should be either the users home dir of
// the marvin++ project root dir
boost::optional<std::string> getEnv(std::string name);
boost::optional<boost::filesystem::path> getEnvMarvinHome();
boost::optional<boost::filesystem::path> getEnvUserHome();
bool validWorkingDir();
bool validEnvVariables();
void setEnvMarvinHome(boost::filesystem::path h);
void setMarvinHomeToUserHome();
void setMarvinHomeToProjectHome();

} // namespace
#endif