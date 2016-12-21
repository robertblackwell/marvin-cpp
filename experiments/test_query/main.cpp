//
//  main.cpp
//  test_query
//
//  Created by ROBERT BLACKWELL on 12/14/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include "query.hpp"

int main(int argc, const char * argv[]) {

    UriQuery q("xy&z=t");
    q.parse("another=1&second=2");
    printMap(q.map());
    auto x = q.map();
    auto y = q;
    return 0;
}
