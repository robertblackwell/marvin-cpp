//
//  Buffer.hpp
//  asio-mitm
//
//  Created by ROBERT BLACKWELL on 11/20/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#ifndef Buffer_hpp
#define Buffer_hpp

#include <stdio.h>
#include <string>
#include <boost/asio.hpp>

class Buffer{
    
public:
    Buffer(std::size_t capacity): capacity_(capacity), content(std::string(capacity, ' '))
    {
    }
    std::string* rawBuffer()
    {
        return &(this->content);
    }
private:
    std::size_t capacity_;
    std::string content;
};
#endif /* Buffer_hpp */
