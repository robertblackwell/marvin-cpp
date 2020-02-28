//
//  marvin_uri.cpp
//  marvin
//
//  Created by ROBERT BLACKWELL on 1/2/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//
#include <marvin/external_src/rb_logger/rb_logger.hpp>
RBLOGGER_SETLEVEL(LOG_LEVEL_WARN)
#include <marvin/message/marvin_uri.hpp>
using namespace Marvin;
namespace Marvin{
    Uri::Uri(std::string uri_in)
    {
        std::string uri(uri_in);
        http::url us = http::ParseHttpUrl(uri);
        LogDebug(" uri:", uri);
        LogDebug(" scheme:", us.protocol);
        LogDebug(" host:", us.host);
        LogDebug(" port:", us.port);
        LogDebug(" path:", us.path);
        LogDebug(" search:", us.search);
        
        std::string host = us.host;
        std::string scheme = us.protocol;
        
        int port_int = us.port;
        if(us.port == 0) {
            if( us.protocol == "https")
                port_int = 443;
            else if( us.protocol == "http")
                port_int = 80;
            else
                assert(false);
        }
        assert(port_int != 0);
        std::string host_header = us.host+":"+std::to_string(port_int);
        if(port_int == 80) {
            host_header = us.host;
        }
        std::string path_value;
        if( (us.path == "") && (us.search == "")) {
            path_value = "/";
        }else if( (us.path == "") && (us.search != "")) {
            path_value = "/?" + us.search;
        } else if( (us.path != "" ) && (us.search == "")){
            path_value = us.path;
        } else {
            path_value = us.path +"?"+ us.search;
        }
        std::string path_abs_value = us.protocol+"://"+host_header+path_value;
        if(us.user != "") {
            m_user = us.user;
            if(us.password != "") {
                m_password = us.password;
                m_user_and_password = m_user +  ": " + us.password;
            }
        }
        m_scheme = us.protocol;
        m_server = us.host;
        m_search = us.search;
        m_host = host_header;
        m_rel_path = path_value;
        m_abs_path = path_abs_value;
        m_port = port_int;
    }
std::string Uri::scheme(){ return m_scheme;}
std::string Uri::user(){ return m_user;}
std::string Uri::password(){ return m_password;}
std::string Uri::userAndPassword(){ return m_user_and_password;}
std::string Uri::host(){ return m_host;}
std::string Uri::server(){ return m_server;}
long        Uri::port(){ return m_port;}
std::string Uri::relativePath(){ return m_rel_path;}
std::string Uri::absolutePath(){ return m_abs_path;}
std::string Uri::search(){ return m_search;}

}
