// Copyright (c) 2017-2020, University of Cincinnati, developed by Henry Schreiner
// under NSF AWARD 1414736 and by the respective contributors.
// All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <CLI/CLI.hpp>
#include <iostream>
#include <string>
#if 1
int main(int argc, char **argv) 
{

    std::string subcmd;
    std::cout << "argv: "; 
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
    bool stop_immediate_flag;
    CLI::Option* stop_immediate = stop->add_flag("-c,--crash", stop_immediate_flag, "Crash stop - Immediate");
    
    app.require_subcommand(1);  // 1 or more


    CLI11_PARSE(app, argc, argv);

    std::vector<CLI::App*> active_subcommands = app.get_subcommands();
    auto x = active_subcommands[0]->get_name();

    int scmd_count =app.get_subcommands().size();
    if (scmd_count <= 0) {
        std::cout << "You must provide a valid subcommand" << std::endl;
        throw("message");
    }
    std::map<std::string, std::function<void()>> jump_table;
    jump_table["https"] = [https]()
    {
        std::cout << "Subcommand: start" << std::endl;

    };
    jump_table["stop"] = [stop, &stop_immediate_flag]()
    {
        if (stop->count("-c")) {
            std::cout << "Subcommand: stop: " << (int)stop_immediate_flag << std::endl;
        } 
        std::cout << "Subcommand: stop" << std::endl;

    };
    std::string name = app.get_subcommands()[0]->get_name();
    (jump_table.at(name))();
return 0;
    auto sub_cmd = app.get_subcommand(0);
    auto opts = sub_cmd->get_options();
    auto file_opt = sub_cmd->get_option_no_throw("--file");
    std::string file_opt_str;
    if (file_opt) {
        file_opt_str = file_opt->as<std::string>();
        std::cout << file_opt_str << std::endl;
        // use it
    }
    return 0;
}
#endif
#if 0
int main(int argc, char* argv[])
{

    CLI::App app("simple test with positional args");
    app.set_help_all_flag("--help-all", "Expand all help");
    app.add_flag("--random", "Some random flag");
    std::vector<std::string> pos;
    app.add_option("positional", pos, "this is a positional argument");

    CLI11_PARSE(app, argc, argv);

    auto x = app.get_option_no_throw("positional")->as<std::vector<std::string>>();

    std::cout << "random : " << app.count("--random") << std::endl;
    std::cout << "" << std::endl;
    for(auto p: pos) {
        std::cout << "positionals : " << p << std::endl;
    }
    return 0;
}
int very_simple(int argc, char **argv) {

    CLI::App app("K3Pi goofit fitter");

    std::string file;
    CLI::Option *opt = app.add_option("-f,--file", file, "File name");

    int count{0};
    CLI::Option *copt = app.add_option("-c,--count", count, "Counter");

    int v{0};
    CLI::Option *flag = app.add_flag("--flag", v, "Some flag that can be passed multiple times");

    double value{0.0};  // = 3.14;
    app.add_option("-d,--double", value, "Some Value");

    std::vector<std::string> pos;
    app.add_option("positional", pos, "this is a positional argument");


    CLI11_PARSE(app, argc, argv);

    std::cout << "Working on file: " << file << ", direct count: " << app.count("--file")
              << ", opt count: " << opt->count() << std::endl;
    std::cout << "Working on count: " << count << ", direct count: " << app.count("--count")
              << ", opt count: " << copt->count() << std::endl;
    std::cout << "Received flag: " << v << " (" << flag->count() << ") times\n";
    std::cout << "Some value: " << value << std::endl;

    return 0;
}

int original(int argc, char **argv) {

    CLI::App app("K3Pi goofit fitter");

    std::string file;
    CLI::Option *opt = app.add_option("-f,--file,file", file, "File name");

    int count{0};
    CLI::Option *copt = app.add_option("-c,--count", count, "Counter");

    int v{0};
    CLI::Option *flag = app.add_flag("--flag", v, "Some flag that can be passed multiple times");

    double value{0.0};  // = 3.14;
    app.add_option("-d,--double", value, "Some Value");

    CLI11_PARSE(app, argc, argv);

    std::cout << "Working on file: " << file << ", direct count: " << app.count("--file")
              << ", opt count: " << opt->count() << std::endl;
    std::cout << "Working on count: " << count << ", direct count: " << app.count("--count")
              << ", opt count: " << copt->count() << std::endl;
    std::cout << "Received flag: " << v << " (" << flag->count() << ") times\n";
    std::cout << "Some value: " << value << std::endl;

    return 0;
}

int main()
{

    char* argv1[] = {"ME", "--help-all"};
    char* argv2[] = {"ME", "--random", "start", "--file", "thefilename"};
    char* argv3[] = {"ME", "simple", "--random", "thefilename", "arg1", "arg2"};
    char* argv4[] = {"ME", "vsimple", "--c", "3", "-f", "thefilename", "-v", "arg1", "arg2"};
    char* argv5[] = {"-v", "-c", "3", "-f", "thefilename", "arg1", "arg2"};
    char* argv6[] = {"simple", "-c", "3", "-f", "thefilename"};
    // test(2, argv1);
    // test_subcommand(5, argv2);
    // test_simple(7, argv3);
    // very_simple(9, argv4);
    original(5, argv4);
    return 0;
}
#endif