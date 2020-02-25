#ifndef helpers_helpers_fs_h
#define helpers_helpers_fs_h

#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <openssl/x509.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
namespace Helpers{

/**
* @brief provides convenience functions for manipulating files and directories. Base on boost::filesystem
*/
namespace fs {

    void combinePEM(boost::filesystem::path dest, boost::filesystem::path src1, boost::filesystem::path src2, std::string tag);
    bool is_regular_file(boost::filesystem::path path);
    bool is_directory(boost::filesystem::path path);
    bool exists(boost::filesystem::path path);
    void create_dir(boost::filesystem::path path);
    void remove_dir(boost::filesystem::path path);
    void file_put_contents(std::string filename, std::string data);
    void file_put_contents(boost::filesystem::path  filename, std::string data);
    std::string file_get_contents(boost::filesystem::path path);

} //namespace fs
} // namespace Helpers
#endif
