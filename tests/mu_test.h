//
//  mu_test.h
//  MarvinCpp
//
//  Created by ROBERT BLACKWELL on 12/4/17.
//  Copyright © 2017 Blackwellapps. All rights reserved.
//

#ifndef mu_test_h
#define mu_test_h

void mu_true(std::string funct, std::string line, int line, bool x);
void mu_eq(std::string funct, std::string line, int line, int x, int y);
void mu_ne(std::string funct, std::string line, int line, int x, int y);

void mu_eq(std::string funct, std::string line, int line, std::string x, std::string y);
void mu_ne(std::string funct, std::string line, int line, std::string x, std::string y);


template<type T>
void mu_eq(std::string funct, std::string line, int line, T x, T y);

template<type T>
void mu_ne(std::string funct, std::string line, int line, T x, T y);

#define ASSERT_TRUE(x) \
    { mu_true(__FUNCTION__, __FILE__, __LINE__, x);}

#define ASSERT_EQ(x,y) \
    { mu_eq(__FUNCTION__, __FILE__, __LINE__, x, y);}

#define ASSERT_NE(x,y) \
    { mu_ne(__FUNCTION__, __FILE__, __LINE__, x, y);}


#endif /* mu_test_h */
