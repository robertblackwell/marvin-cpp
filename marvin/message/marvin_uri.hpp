//
//  marvin_uri.hpp
//  marvin
//
//  Created by ROBERT BLACKWELL on 1/2/18.
//  Copyright © 2018 Blackwellapps. All rights reserved.
//

#ifndef marvin_uri_hpp
#define marvin_uri_hpp
#include <string>
#include <cassert>
#include <marvin/external_src/uri/UriParser.hpp>
namespace Marvin{
    class Uri;
    using UriSPtr = std::shared_ptr<Uri>;
    class Uri
    {
        public:
            Uri(std::string uri_string);
            /// \brief either "http" or "https"
            std::string scheme();
            /// \brief if the uri had a username that value is returned otherwise ""
            std::string user();
            /// \brief if the uri had a user passwordthat value is returned otherwise ""
            std::string password();
            /// \brief if the uri had both a username and a  password the value "user:password" is returned ortherwise ""
            std::string userAndPassword();
            /// \brief returns a host value suitable for a host header in the form somewhere.com:port - the port is always present
            std::string host();
            /// \brief the host name without the port attached suitable for boost::asio resolver
            std::string server();
            /// \brief interger port number always has a value - 0 is an error and you have found a bug
            long        port();
            /// \brief returns the relative path of the uri, maybe a bad name, a relative uri,
            /// in the example http://somewhere.com/tone/two?a=111#fragment this method returns
            /// /tone/two?a=111#fragment.
            std::string relativePath();
            /// \brief again maybe a poor name - returns the entire uri as suitable for the first line
            /// of an http request so the user and password have been removed
            std::string absolutePath();
            /// \brief returns everything after the "?" including the fragment
            std::string search();
        private:
            std::string m_scheme;
            std::string m_user;
            std::string m_user_and_password;
            std::string m_password;
            std::string m_host;
            std::string m_server;
            long        m_port;
            std::string m_rel_path;
            std::string m_abs_path;
            std::string m_search;
            http::url m_uri;
    };
}
#endif /* marvin_uri_hpp */
