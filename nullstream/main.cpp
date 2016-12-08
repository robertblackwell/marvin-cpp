//
//  main.cpp
//  nullstream
//
//  Created by ROBERT BLACKWELL on 12/7/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>

class NulStreambuf : public std::streambuf
{
    char                dummyBuffer[64];
protected:
    virtual int         overflow( int c )
    {
        char ch = (char)c;
        setp( dummyBuffer, dummyBuffer + sizeof( dummyBuffer ) ) ;
        return (c == EOF) ? '\0' : c ;
    }
};

class NulOStream : public NulStreambuf, public std::ostream
{
public:
    NulOStream() : std::ostream( this ) {}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    
    NulOStream noS;
    
    noS << "Hello, World!\n";
    std::cout << "Hello, World!\n";
    return 0;
}
