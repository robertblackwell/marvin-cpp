// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include <doctest/doctest.h>

// int factorial(int number) { return number <= 1 ? number : factorial(number - 1) * number; }

// TEST_CASE("testingthefactorialfunction") {
//     std::cout << "testing doctest" << std::endl;
//     CHECK(factorial(1) == 1);
  
//     CHECK(factorial(2) == 2);
//     // CHECK(factorial(3) == 16);
//     CHECK(factorial(10) == 3628800);
// }

#include <cstdio>
#include <unistd.h>
int main() {
    char buf[1000];
    char* b = getcwd(buf, 1000);
    printf("Hello got here cwd = %s\n", b);
    printf("Hello got here\n");
    printf("Hello got here\n");
    printf("Hello got here\n");
    printf("Hello got here\n");
}