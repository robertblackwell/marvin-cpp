#ifndef marvin_guard_app_handshake_check_hosts_hpp
#define marvin_guard_app_handshake_check_hosts_hpp
#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <cert/cert.hpp>
#include <marvin/certificates/certificates.hpp>

namespace po = boost::program_options;
namespace bf = boost::filesystem;

///
/// a group of functions that check the host name against 
/// the CN and DNS names in the subjectAltName extension
///

/// split a string by a delimiter char
std::vector<std::string> split_string(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

std::string replace_substring_all(std::string target, std::string substr_to_match, std::string replacement)
{
    std::string result = boost::replace_all_copy(target , substr_to_match , replacement);
    const std::string s = "*A";
    const std::string t = "*A\n";

    std::string::size_type n = 0;
    while ( ( n = target.find( s, n ) ) != std::string::npos )
    {
        target.replace( n, s.size(), t );
        n += t.size();
    }
}
///
/// splits a string like "DNS:*.google.com, "DNS:something.google.com, ... " 
///into {"*.google.com", "somthing.google.com"}
///
/// Complication: the original string may have elements that start with 
/// ofther than DNS - we want those gone
///
std::vector<std::string> splitDNSNames(std::string dns_names)
{
    std::string target = dns_names;
    std::string substr_to_match = "DNS:";
    std::string replacement = "";
    /// remove spaced
    std::string dns_names_string_no_spaces = boost::replace_all_copy(target ," " , "");
    /// split on ','
    std::vector<std::string> dns_name_tokens = split_string(dns_names_string_no_spaces, ',');
    /// remove dns: from front of 
    std::vector<std::string> cleaned_up_dns_names;
    for (auto ent: dns_name_tokens) {
        std::string after_lower = boost::to_lower_copy(ent);
        if (after_lower.find("dns:") != std::string::npos) {
            // if its a dns entry remove the dns
            std::string s = boost::replace_all_copy(after_lower, "dns:", "");
            cleaned_up_dns_names.push_back(s);
        }
    }
    return cleaned_up_dns_names;
}
///
/// tests to see if a name like "google.com" (no wildcards) matches
/// against a name like "*.google.com"
///
bool matchHostAgainstDnsName(std::string host, std::string dns_name)
{
    std::vector<std::string> res_host = split_string(host, '.');
    std::vector<std::string> res_dns = split_string(dns_name, '.');
    if (res_host.size() > res_dns.size()) {
        return false;
    }
    bool match = false;
    int j_dns;
    for(int i = 0; i < res_host.size(); i++) {
        int j_host = res_host.size() - 1 -i;
        j_dns = res_dns.size() - 1 - i;
        std::string s_host = res_host[j_host];
        std::string s_dns = res_dns[j_dns];
        // match = ((res_host[j_host] == res_dns[j_dns]) || (res_dns[j_dns] == "*"));
        if (res_host[j_host] == res_dns[j_dns]) {
            // keep going
            match = true;
            if ((j_host == 0) && (j_dns > 0) && (res_dns[j_dns - 1] != "*")) {
                return false;
            }
        } else if ((res_dns[j_dns] != "*") &&((res_host[j_host] != res_dns[j_dns]))) {
            // definite no match
            return false;
        } else if (res_dns[j_dns] != "*") {
            // wild card - must be left most element of res_dns
            // and left most element of res_host
            if ((j_host == 0) && (j_dns == 0)) {
                return true;
            }
            return false;
        } else {

        }
    }
    return match;
}
///
/// tess a host name such as "google.com" against a string of names like
/// "DNS:*.google.com, DNS: *.another.google.com"
///
bool matchHostAgainstDnsAltString(std::string host, std::string altNames)
{
    std::vector<std::string> dns_patterns = splitDNSNames(altNames);
    for(int i = 0; i < dns_patterns.size(); i++) {
        if(matchHostAgainstDnsName(host, dns_patterns[i])){
            return true;
        }
    }
    return false;
}
#endif