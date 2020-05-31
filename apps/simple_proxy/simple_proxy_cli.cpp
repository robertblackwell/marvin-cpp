#include <trog/loglevel.hpp>
#define TROG_FILE_LEVEL (TROG_LEVEL_TRACE3)
#include <marvin/configure_trog.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <boost/process.hpp>
#include <regex>
#include <CLI/CLI.hpp>
#include <marvin/certificates/certificates.hpp>
#include "ctl_thread.hpp"
#include "mitm_thread.hpp"



using ControlRequestS = std::string;
struct ControlRequest
{
    std::string method;
    std::string path;
};
struct ControlResponse {
    int                         return_code;
    std::vector<std::string>    output;
// using ControlResponse = std::vector<std::string>;
};

using CommandId = std::string;
using CommandHandler = std::function<ControlRequest()>;

std::map<CommandId, CommandHandler> jump_table;


void register_handler(CommandId cmd, CommandHandler h)
{
    jump_table[cmd] = h;
}
ControlRequest call_handler(CommandId cmd)
{
    if (jump_table.find(cmd) == jump_table.end()) {
        throw "command : " + cmd + " not found";
    }
    return (jump_table.at(cmd))();
}

ControlResponse send_request(ControlRequest req)
{
    namespace bp = boost::process;
    bp::pipe p;
    bp::ipstream is;

    std::vector<std::string> outline;

    //we just use the same pipe, so the 
    std::vector<std::string> cmd_list;
    std::string url = "http://localhost:9993"+req.path;
    // url = "http://whiteacorn.com";

    bp::child curl(bp::search_path("curl"), "-X", req.method, "-sS", "--raw", url, bp::std_out > is);

    std::string line;
    while (curl.running() && std::getline(is, line)) //when nm finished the pipe closes and c++filt exits
        outline.push_back(line);

    curl.wait();
    int result = curl.exit_code();
    ControlResponse curl_resp = {result, outline};
    return curl_resp;
}

void display_response(ControlResponse resp)
{
    std::cout << "Response return code is : " << resp.return_code << std::endl;
    if (resp.return_code == 0) {
        for(std::string line: resp.output) {
            std::cout << line << std::endl;
        }
    } else {
        std::cout << "Request failed " << std::endl;
    }
}

namespace bf = boost::filesystem;
int main(int argc, char **argv) 
{

    std::string subcmd;
    std::cout << "argv: ";
    std::string filter_id_number;
    std::string filter_regex;

    for(int i = 0; i < argc; i++) {
        std::cout << " " << argv[i]; 
    }
    std::cout << std::endl;
    CLI::App app("simple proxy control");
    app.set_help_all_flag("--help-all", "Expand all help");
    app.add_flag("-D,--debug", "Some random flag");
    app.add_flag("-V,--version", "Version Number");

    CLI::App *https = app.add_subcommand("start","A great subcommand");
    std::string regex;
    CLI::Option* start_regex = https->add_option("-x,--regex", regex, "A regex for slecting https hosts");

    CLI::App *stop = app.add_subcommand("stop", "Stop the proxy");

    // filter commands
    CLI::App *filter_list = app.add_subcommand("filter_list", "list filter regexes");
    CLI::App *filter_remove = app.add_subcommand("filter_remove", "remove 1 or all filter regexes");
    filter_remove->add_option("id_number", filter_id_number, "Number(from filter_list command) that identifies a regex or * or 'all' to remove all")->required();
    CLI::App *filter_add = app.add_subcommand("filter_add", "add filter regexes");
    filter_add->add_option("filter_regex", filter_regex, "A regular expression")->required();
    // display body commands
    CLI::App *msg_bodies_on = app.add_subcommand("bodies_on", "display message bodies");
    CLI::App *msg_bodies_off = app.add_subcommand("bodies_off", "do not display message bodies");

    bool stop_immediate_flag;
    CLI::Option* stop_immediate = stop->add_flag("-c,--crash", stop_immediate_flag, "Crash stop - Immediate");
    
    app.require_subcommand(1);  // 1 or more


    CLI11_PARSE(app, argc, argv);

    std::vector<CLI::App*> active_subcommands = app.get_subcommands();
    auto x = active_subcommands[0]->get_name();

    int scmd_count = app.get_subcommands().size();
    if (scmd_count <= 0) {
        std::cout << "You must provide a valid subcommand" << std::endl;
        throw("message");
    }
    register_handler("stop", [stop, &stop_immediate_flag]() -> ControlRequest {
        if (stop->count("-c")) {
            std::cout << "Subcommand: stop: " << (int)stop_immediate_flag << std::endl;
        } 
        std::cout << "Subcommand: stop" << std::endl;
        return {.method="GET", .path="/stop"};
    });
    register_handler("filter_list", []() -> ControlRequest {
        std::cout << "Subcommand: filter_list" << std::endl;
        return {.method="GET", .path="/filter_list"};
    });
    register_handler("filter_remove", [filter_id_number]() -> ControlRequest {
        std::cout << "Subcommand: filter_remove" << std::endl;
        return {.method="DELETE", .path="/filter_remove/"+filter_id_number};
    });
    register_handler("filter_add", [filter_regex]() -> ControlRequest {
        std::cout << "Subcommand: filter_add" << std::endl;
        return {.method="POST", .path="/filter_add/"+filter_regex};
    });
    register_handler("bodies_off", [stop, &stop_immediate_flag]() -> ControlRequest {
        std::cout << "Subcommand: bodies_off" << std::endl;
        return {.method="POST", .path="/bodies_off"};
    });
    register_handler("bodies_on", []() -> ControlRequest {
        std::cout << "Subcommand: stop" << std::endl;
        return {.method="POST", .path="/bodies_on"};
    });
    std::string name = app.get_subcommands()[0]->get_name();
    ControlRequest req = (jump_table.at(name))();
    
    display_response(send_request(req));

    return 0;
}