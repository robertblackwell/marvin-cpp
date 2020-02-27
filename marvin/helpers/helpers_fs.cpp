#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <marvin/helpers/error.hpp>
#include <marvin/helpers/helpers_fs.hpp>

using namespace Helpers::fs;

void Helpers::fs::combinePEM(boost::filesystem::path dest, boost::filesystem::path src1, boost::filesystem::path src2, std::string tag)
{
    std::string src1_str = file_get_contents(src1);
    std::string src2_str = file_get_contents(src2);
    std::string tmpl = "\n%1%\n========================\n";
    std::string s = str(boost::format(tmpl) % tag);
    std::string res = src1_str + s + src2_str;
    file_put_contents(dest, res);
}

bool Helpers::fs::is_regular_file(boost::filesystem::path path)
{
    struct stat path_stat;
    const char* cpath = path.string().c_str();
    stat(cpath, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
bool Helpers::fs::is_directory(boost::filesystem::path path)
{
    struct stat path_stat;
    const char* cpath = path.string().c_str();
    stat(cpath, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
bool Helpers::fs::exists(boost::filesystem::path path)
{
    struct stat   buffer;
    const char* filename = path.string().c_str();
    int res = stat(filename, &buffer);
    return (res == 0);
}
void Helpers::fs::create_dir(boost::filesystem::path path)
{
    struct stat st{}; // = {0};
    const char* cpath = path.string().c_str();

    if (stat(cpath, &st) == -1) {
        int e = mkdir(cpath, 0700);
        if (e == -1) {
            THROW( __func__  << " failed errno: " << errno) ;
        }
    } else {
        THROW(" path_create_dir: failed for path " << path.string());
    }
}
void Helpers::fs::remove_dir(boost::filesystem::path path)
{
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-variable"
    auto fcount = boost::filesystem::remove_all(path);
    #pragma clang diagnostic pop

}
void Helpers::fs::file_put_contents(std::string filename, std::string data)
{
    std::ofstream outfile(filename);
    if (outfile.fail()) {
        THROW("filename: [" << filename << "]  probably does not exist");
    }
    outfile << data;
    outfile.close();
}
void Helpers::fs::file_put_contents(boost::filesystem::path  filename, std::string data)
{
    file_put_contents(filename.string(), data);
}
std::string Helpers::fs::file_get_contents(boost::filesystem::path path)
{
    std::ifstream fin(path.string());
    std::stringstream buffer;
    buffer << fin.rdbuf();
    std::string result = buffer.str();
    return result;
}
