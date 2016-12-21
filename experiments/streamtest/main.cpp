//
//  main.cpp
//  streamtest
//
//  Created by ROBERT BLACKWELL on 12/8/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    std::string s;
    std::stringbuf b;
    b.str(s);
    b.sputn ("255 in hexadecimal: ",20);
    std::ostream os(&b);
    os << "this is some stuff" ;
    std::cout << "Goodbye";
    
    
    return 0;
}
