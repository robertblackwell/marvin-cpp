//
//  marvin_config.hpp
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 1/13/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//

#ifndef marvin_config_hpp
#define marvin_config_hpp
/*
    {
        "listen_port" : "9991",
        "max_connections" : 35,
        "thread_pool_max" : 1,
        "buffer_size" :{
            "m_buffer_min" : 1000,
        },
        "ssl_ports" : ["443","9443"],
        "ssl_hosts" :{
            "https://ssllabs.com:443"
        }
        "connection_timeouts {
            "connect" : 30000,
            "read" : 30000,
            "write" : 30000,
        }
        tunnel_timeouts {
            first_read : 60000
            subsequent_read : 15000
        },
        "ssl" : {
            "cert_path" : "/Users/rob/CA/allroots"
        }
    }
*/
#include <stdio.h>
namespace Marvin {
    class Config {
    
    };
}
#endif /* marvin_config_hpp */
