//
//  main.cpp
//  testcase
//
//  Created by ROBERT BLACKWELL on 12/8/16.
//  Copyright © 2016 Blackwellapps. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <cassert>
#include "tcase.hpp"
//
// raw mode
//
bool test1() {
    auto tc = new Testcase(
                            // raw message text
                            std::vector<std::string> {
                                "HTTP/1.1 200 OK 11Reason Phrase",
                                "Host: ahost",
                                "Connection: keep-alive",
                                "Proxy-Connection: keep-alive",
                                "Content-length: 11",
                                "01234567890",
                                ""
                            },
                            // expected first line
                            std::string("HTTP/1.1 200 OK 11Reason Phrase"),
                            // expexted headers
                            std::vector< std::vector< std::string > >{
                                std::vector<std::string>{"Host", "ahost"},
                                std::vector<std::string>{"Connection","keep-alive"},
                                std::vector<std::string>{"Proxy-Connection","keep-alive"},
                                std::vector<std::string>{"Content-length","11"}
                            },
                            
                            // expected body
                            std::string("01234567890")
                           );
    return true;
}
void stringDiff(std::string s1, std::string s2){
    if( s1.size() != s2.size() )
        std::cout << "s1.size: " << s1.size() << " s2.size(): " << s2.size() << std::endl;
    for(int i = 0; i < s1.size(); i++){
        if( s1.c_str()[i] != s2.c_str()[i] ){
            std::cout << "differ at position " << i << std::endl;
            std::cout << "s1: " << s1.c_str()[i] << " s2: " << s2.c_str()[i] << std::endl;
        }
    }
}
//
// in time we want this to run the parser
//
std::string processTestcase(Testcase tc){
    std::ostringstream ss;
    while(! tc.finished()){
        std::string s = tc.next();
        ss << s;
        std::string tm = ss.str();
    }
    std::string s1 = ss.str();
    return s1;
}
//
// Non raw mode
//

//bool verifyTestcase(Testcases& tcs, int index)
//{
//    Testcase tc = tcs.getCase(index);
//    std::string s1 = processTestcase(tc);
//    std::string s2 = tc.result();
//    bool ok (s1 == s2);
//    if( ! ok ){
//        std::cout << "[" << s1 << "]" << std::endl;
//        std::cout << "[" << s2 << "]" << std::endl;
//        stringDiff(s1,s2);
//    }
//    return ok;
//}

bool test2() {
    auto tc = Testcase(
               // raw message text
               std::vector<std::string> {
                   "HTTP/1.1 200 OK 11Reason Phrase",
                   "Host: ahost",
                   "Connection: keep-alive",
                   "Proxy-Connection: keep-alive",
                   "Content-length: 11",
                   "01234567890",
                   ""
               },
               // expected first line
               std::string("HTTP/1.1 200 OK 11Reason Phrase"),
               // expexted headers
               std::vector< std::vector< std::string > >{
                   std::vector<std::string>{"Host", "ahost"},
                   std::vector<std::string>{"Connection","keep-alive"},
                   std::vector<std::string>{"Proxy-Connection","keep-alive"},
                   std::vector<std::string>{"Content-length","11"}
               },
               
               // expected body
               std::string("01234567890")
   );

    return true;
}
void testCreationOfTestcase(){
    
    auto tc = Testcase(
             // raw message text
             std::vector<std::string> {
                 "HTTP/1.1 200 OK 11Reason Phrase",
                 "Host: ahost",
                 "Connection: keep-alive",
                 "Proxy-Connection: keep-alive",
                 "Content-length: 11",
                 "01234567890",
                 ""
             },
             // expected first line
             std::string("HTTP/1.1 200 OK 11Reason Phrase"),
             // expexted headers
             std::vector< std::vector< std::string > >{
                 std::vector<std::string>{"Host", "ahost"},
                 std::vector<std::string>{"Connection","keep-alive"},
                 std::vector<std::string>{"Proxy-Connection","keep-alive"},
                 std::vector<std::string>{"Content-length","11"}
             },
             
             // expected body
             std::string("01234567890")
    );
    printf("");
}
void createTestcases(){
    Testcases tcs;
    auto x = tcs.getCase(0);
    printf("");
}
//void verifyTestcases(){
//    Testcases tcs;
//    int num_test_cases = tcs.numberOfTestcases();
//    for(int ix = 0; ix < num_test_cases; ix++){
//        bool r = verifyTestcase(tcs, ix);
//        std::cout << "Testcase : " << ix << " result : " << r << std::endl;
//    }
//    
//}

int main(int argc, const char * argv[]) {
//    assert(test1());
//    assert(test2());
    testCreationOfTestcase();
    createTestcases();
//    verifyTestcases();
}
