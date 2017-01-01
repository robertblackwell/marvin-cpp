//
//  main.cpp
//  ini-test
//
//  Created by ROBERT BLACKWELL on 12/31/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string ini_fn = "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/ini-test/test.ini";
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(ini_fn, pt);
    std::cout << pt.get<std::string>("section.key1") << std::endl;
    std::cout << pt.get<std::string>("anothersection.key1") << std::endl;
    std::cout << pt.get<std::string>("section.key2") << std::endl;
    
    std::string json_fn = "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/ini-test/test.json";
    boost::property_tree::ptree pt_json;
    boost::property_tree::json_parser::read_json(json_fn, pt_json);
    auto c = pt_json.get_child("array").get<std::string>("");
    std::cout << pt_json.get<std::string>("key1") << std::endl;
    std::cout << pt_json.get<std::string>("key2.sub1") << std::endl;
    
    
    std::string info_fn = "/Users/rob/MyCurrentProjects/Pixie/MarvinCpp/ini-test/test.info";
    boost::property_tree::ptree pt2;
    boost::property_tree::info_parser::read_info(info_fn, pt2);
    std::cout << pt2.get<std::string>("key1") << std::endl;
    std::cout << pt2.get<std::string>("key2") << std::endl;
    std::cout << pt2.get<std::string>("key2.key3") << std::endl;
    std::cout << pt2.get<std::string>("array") << std::endl;
    
    return 0;
}
