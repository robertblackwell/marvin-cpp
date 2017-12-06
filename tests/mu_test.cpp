#include mu_test.h

std::string mu_prefix(std::string funct, std::string file,  int line)
{
    ostringstream os;
    os << "func: " << funct << " file: " << file << "[" << line << "]" ;
    return os.str();
}

void mu_true(std::string funct, std::string file, int line, bool x)
{
    ostringstream os;
    
}
void mu_eq(std::string funct, std::string file, int line, int x, int y)
{

}
void mu_ne(std::string funct, std::string file, int line, int x, int y)
{

}

void mu_eq(std::string funct, std::string file, int line, std::string x, std::string y)
{

}
void mu_ne(std::string funct, std::string file, int line, std::string x, std::string y)
{

}


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
