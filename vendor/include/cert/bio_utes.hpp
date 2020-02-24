//
//  bio_utes.hpp
//  x509
//
//  Created by Robert BLACKWELL on 2/8/20.
//  Copyright © 2020 ROBERT BLACKWELL. All rights reserved.
//

#ifndef bio_utes_hpp
#define bio_utes_hpp

#include <stdio.h>
/**
* If bio is a memory bio that has been written to this function will convert the
* bio's internal buffer to a std::string
 */
std::string BIO_to_string(BIO* bio);

size_t BIO_mem_length(BIO* bio);
#endif /* bio_utes_hpp */
