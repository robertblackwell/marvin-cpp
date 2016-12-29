//
//  main.cpp
//  regexs
//
//  Created by ROBERT BLACKWELL on 12/28/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <regex>


int main(int argc, const char * argv[]) {
    // insert code here...
    std::regex re("^text\\/(.)*$");
    std::string s("texthtml");
    auto x = std::regex_search(s, re);
    std::cout << "Hello, World!\n";
    return 0;
}
