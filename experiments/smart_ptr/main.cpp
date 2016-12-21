//
//  main.cpp
//  smart_ptr
//
//  Created by ROBERT BLACKWELL on 12/12/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <memory>
class Thing
{
public:
    Thing(){};
    ~Thing(){
        std::cout <<" ~Thing" << std::endl;
    }
    int i1, i2, i3,i4;
};
class Test
{
public:
    Test()
    {
        thing_ptr = std::shared_ptr<Thing>(new Thing());
    }
    ~Test(){
        std::cout << "~Test" << std::endl;
    }
    std::shared_ptr<Thing> thing_ptr;
};

void capture(std::shared_ptr<Test> test_ptr){
    std::weak_ptr<Test> tt = test_ptr;
    int i = 2;
}

int main(int argc, const char * argv[]) {

    std::shared_ptr<Test> sp = std::shared_ptr<Test>(new Test());
    capture(sp);
    sp.reset();
    // insert code here...
    std::cout << "Hello, World!\n";
    
    return 0;
}
