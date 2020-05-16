//
// Start both a server and a number of client tthreads and have each of the client
// threads make a number of requests from the server.
//
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <unistd.h>
#include <thread>
#include <pthread.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <marvin/boost_stuff.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <marvin/connection/socket_factory.hpp>
#include <marvin/http/message_base.hpp>
#include <marvin/client/client.hpp>
#include <marvin/helpers/mitm.hpp>

#include <marvin/configure_trog.hpp>
TROG_SET_FILE_LEVEL(Trog::LogLevelWarn)

#include <marvin/http/uri.hpp>

using namespace Marvin;
using namespace std;

// some experiments in references and rvalue references

struct BoVector;
void print(std::string label, BoVector v);
#define MPRINT(v) \
    do {    \
        std::cout << __PRETTY_FUNCTION__ \
        << " " << std::hex << v.arr << std::dec \
        << " " << v.my_counter \
        << " " << v.size \
        << std::endl; \
    } while(0);

static int counter = 0;
struct BoVector
{
    int my_counter;
    int size;
    double* arr;
    BoVector(int capacity)
    {
        my_counter = ++counter;
        size = capacity;
        MPRINT((*this));
        arr = new double[capacity];
    }
    BoVector(const BoVector& rhs)
    {
        my_counter = ++counter;
        MPRINT(rhs);
        size = rhs.size;
        arr = new double[size];
        for(int i=0;i<size;i++) {
            arr[i] = rhs.arr[i];
        }
        MPRINT((*this));
    }
    BoVector(BoVector&& rhs)
    {
        my_counter = ++counter;
        MPRINT(rhs);

        size = rhs.size;
        arr = rhs.arr;
        rhs.arr = nullptr;
        rhs.size = 0;
        MPRINT((*this))
    }

#if 1
    BoVector& operator =(const BoVector& rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        size = rhs.size;
        arr = new double[size];
        for(int i=0;i<size;i++) {
            arr[i] = rhs.arr[i];
        }
        return *this;
    }
    BoVector& operator =(BoVector&& rhs)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        size = rhs.size;
        arr = rhs.arr;
        rhs.arr = nullptr;
        rhs.size = 0;
        return *this;
    }
#endif
    ~BoVector()
    {
        MPRINT((*this))
        delete arr;
    };
};
void print(std::string label, BoVector& v)
{
    std::cout << label
              << " " << std::hex << v.arr << std::dec
              << " " << v.my_counter
              << " " << v.size
              << std::endl;
}

void fooRef(BoVector const& v)
{
    MPRINT(v)
}
void foo(BoVector v)
{
    MPRINT(v)
}
BoVector decorator(BoVector v)
{
    MPRINT(v);
    return v;
}
BoVector decoratorDoubleRef(BoVector&& v)
{
    MPRINT(v);
    return v;
}
BoVector decoratorRef(BoVector& v)
{
    MPRINT(v);
    return v;
}
BoVector createBoVector(int cap)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    BoVector v{cap};
    return v;
}
TEST_CASE("bovector - RVO")
{
    std::cout << "RVO====================================================" << std::endl;
    // NOTE: the way to see how many instances have been created is to count the number of
    // calls to the destructor
    // this example creates 3 BoVector objects.
    // THe initial creation of one with capacity 50 on the next line
    BoVector reusable = createBoVector(50);
    // another copy is created upon passing into foo()
    foo(reusable);
    // interestingly this next line creates only 1 instance of capacity 50
    // and that is surprising as we would have naivley expected on
    // from the new inside createBoVector and one on the return
    // however the return is avoided by RVO - return value optimization.
    foo(createBoVector(100));
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
TEST_CASE("bovector no-RVO")
{
    std::cout << "NO-RVO====================================================" << std::endl;
    // this example creates 4 BoVector objects - unlike the previous case.
    // The initial creation of one with capacity 50 on the next line
    BoVector reusable = createBoVector(52);
    // a call to the copy constructor for this call makes it two objects
    foo(reusable);
    // interestingly this next line creates 2 instance
    // even though we are passing in an existing object
    // Note there is a call to both the copy and the move constructor
    // for this call. The copy on the way in and the move on the return.
    // But NO RVO - that is because C++ does not RVO when the object being returned was an argument.
    foo(decorator(reusable));
    // also note by looking at the print out that one of the intances has been moved
    // as it has a size==0 - its the returned temporary inside decorator()
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
TEST_CASE("bovector fooRef")
{
    std::cout << "fooRef====================================================" << std::endl;
    BoVector reusable = createBoVector(52);
    // the use of fooRef() saves us one instance
    // only two instance in total for this example
    fooRef(reusable);
    foo(createBoVector(102));
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
TEST_CASE("bovector fooRef+decorate")
{
    std::cout << "fooRef====================================================" << std::endl;
    BoVector reusable = createBoVector(52);
    // the use of fooRef() saves us one instance
    fooRef(reusable);
    // but this line creates two instances - in print out see both
    // the copy and move constructor called
    // this test has three instances
    foo(decorator(reusable));
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}
TEST_CASE("bovector fooRef+decoratorref")
{
    std::cout << "decoratorRef====================================================" << std::endl;
    BoVector reusable = createBoVector(52);
    // the use of fooRef() saves us one instance
    fooRef(reusable);
    // using the double ref and std::move saves us an instance
    // notice only two instances for this experiment
    // The ref argument on decoratorRef saves a copy
    // This is the most efficient and simplest solution
    // for this exercise
    foo(decoratorRef(reusable));
    std::cout << __PRETTY_FUNCTION__
              << " reusable.arr: " << std::hex << reusable.arr << std::dec
              << " reusable.my_counter: " << reusable.my_counter
              << " reusable.size: "<< reusable.size << std::endl;
}
TEST_CASE("bovector fooRef+decoratordoubleref")
{
    std::cout << "decoratordoubleRef====================================================" << std::endl;
    BoVector reusable = createBoVector(52);
    // the use of fooRef() saves us one instance
    fooRef(reusable);
    // using the double ref and std::move saves us an instance
    // notice only two instances for this experiment
    // AND resuable has not been emptied by the use of std::move
    // also note only two constructires were called in this experiment
    // BoVector(capacity) once
    // BoVector(BoVector&) once// so the use of std::move saves a copy but
    // DOES not cause a call to the move constructor
    foo(decoratorDoubleRef(std::move(reusable)));
    std::cout << __PRETTY_FUNCTION__
        << " reusable.arr: " << std::hex << reusable.arr << std::dec
        << " reusable.my_counter: " << reusable.my_counter
        << " reusable.size: "<< reusable.size << std::endl;
}
std::string mvstring(std::string in_str)
{
    return std::move(in_str);
}
struct field {
    std::string key;
    std::string value;
    field(std::string_view k, std::string_view v)
    :   key(std::string{k.data(), k.size()}),
        value(std::string{v.data(), v.size()})
    {

    }

#ifdef XYZ
    field(std::string k, std::string v): key(k), value(v){}
#else
    field(std::string k, std::string v): key(std::move(k)), value(std::move(v)){}
#endif
};
void print_field(field const& f)
{
    std::cout << "===========================================================" << std::endl;
    std::cout << "field.key : " << f.key<< std::endl;
    std::cout << "f.key c_str : " << std::hex << (void*)f.key.c_str() << std::dec << std::endl;
    std::cout << "f.key capacity : " << f.key.capacity() << std::endl;
    std::cout << "f.key size : " << f.key.size() << std::endl;
    std::cout << "field.value : " << f.value<< std::endl;
    std::cout << "f.value c_str : " << std::hex << (void*)f.value.c_str() << std::dec << std::endl;
    std::cout << "f.value capacity : " << f.value.capacity() << std::endl;
    std::cout << "f.value size : " << f.value.size() << std::endl;
    std::cout << "===========================================================" << std::endl;
}
TEST_CASE("reserve 10000")
{
    int const number = 100000;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < number; i++) {
        std::string s; s.reserve(1000);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    auto x = duration.count();
    std::cout << "reserve 1000 100,000" << " duration(nanosecs): " << duration.count() << std::endl;
    std::cout << "reserve 1000 100,000" << " duration(nanosecs): " << double(duration.count())/(float)(number) << std::endl;
}
TEST_CASE("str:;string copy 10000")
{
    int const number = 100000;
    char ch='x';
    std::string s(std::size_t(1000), ch);
    std::string s2;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < number; i++) {
        s2 = std::move(s+"x");
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    auto x = duration.count();
    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << duration.count() << std::endl;
    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << double(duration.count())/(float)(number) << std::endl;
}

TEST_CASE("string swap")
{
    std::string s2{"01234567890000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"};
    std::cout << "s2 : " << s2<< std::endl;
    std::cout << "s2 c_str : " << std::hex << (void*)s2.c_str() << std::dec << std::endl;
    std::cout << "s2 capacity : " << s2.capacity() << std::endl;
    std::cout << "s2 size : " << s2.size() << std::endl;
    void* ptr2 = (void*) s2.c_str();
    std::string s1{std::move(s2)};
    void* ptr1 = (void*) s1.c_str();
    CHECK(ptr1 == ptr2);
//    s1.reserve(1000);
    std::cout << "s1 : " << s1<< std::endl;
    std::cout << "s1 c_str: " << std::hex << (void*)(s1.c_str()) << std::dec << std::endl;
    std::cout << "s1 capacity: " << s1.capacity() << std::endl;
    std::cout << "s1 size : " << s1.size() << std::endl;
//    s1 = mvstring(s2);
//    s1 = std::move(s2);
//    std::swap(s1,s2);
    std::cout << "s2 : " << s2<< std::endl;
    std::cout << "s2 c_str: " << std::hex << (void*)s2.c_str() << std::dec  << std::endl;
    std::cout << "s2 capacity: " << s2.capacity() << std::endl;
    std::cout << "s2 size : " << s2.size() << std::endl;
    std::cout << "s1 : " << s1<< std::endl;
    std::cout << "s1 c_str: " << std::hex << (void*)s1.c_str() << std::dec  << std::endl;
    std::cout << "s1 capacity: " << s1.capacity() << std::endl;
    std::cout << "s1 size : " << s1.size() << std::endl;
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    field f(std::move(s1), "thisisavalue");
    std::cout << "key : " << f.key << std::endl;
    std::cout << "key c_str: " << std::hex << (void*)f.key.c_str() << std::dec  << std::endl;
    std::cout << "key capacity: " << f.key.capacity() << std::endl;
    std::cout << "key size : " << f.key.size() << std::endl;
    void* ptr_key = (void*)f.key.c_str();
    CHECK(ptr2 == ptr_key);
    CHECK(ptr1 == ptr_key);
    print_field(f);
}
TEST_CASE("field speed")
{
    int const number = 100000;
    std::vector<field> lines; lines.reserve(100000);
    std::string key_buffer = "akey";
    std::string value_buffer(std::size_t(200), 'x');
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < number; i++) {
        std::string key_buffer = "akey";
        std::string value_buffer(std::size_t(200), 'x');
        #ifdef XYZ
            lines.emplace_back(field(key_buffer, value_buffer));
        #else
            lines.emplace_back(field(std::move(key_buffer), std::move(value_buffer)));
        #endif
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << duration.count() << std::endl;
    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << double(duration.count())/(float)(number) << std::endl;

}
TEST_CASE("field speed vec of pointers")
{
    namespace b = boost;
    int const number = 100000;
#define MARVBOOST
#ifdef MARVBOOST
    b::ptr_vector<field> lines; lines.reserve(100000);
#else
    std::vector<field> lines; lines.reserve(100000);
#endif
    std::string key_buffer = "akey";
    std::string value_buffer(std::size_t(200), 'x');
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < number; i++) {
        std::string key_buffer = "akey";
        std::string value_buffer(std::size_t(200), 'x');
#ifdef XYZ
        lines.emplace_back(field(key_buffer, value_buffer));
#else
#ifdef MARVBOOST
        lines.push_back(new field(std::move(key_buffer), std::move(value_buffer)));
#else
        lines.emplace_back(field(std::move(key_buffer), std::move(value_buffer)));
#endif
#endif
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << duration.count() << std::endl;
    std::cout << "copy string 1000 100,000" << " duration(nanosecs): " << double(duration.count())/(float)(number) << std::endl;

}
namespace rb {
struct true_type
{
    static constexpr bool value = true;
};
struct false_type
{
    static constexpr bool value = false;
};
static_assert(rb_true_type::value, "This did not work");

template<typename T, typename U>
struct is_same : false_type
{
};
template <typename T>
struct is_same<T, T> : true_type {};

static_assert(!is_same<int, char>::value, "***"); // ok
static_assert(is_same<int, int>::value, "***");  // fires!

template <typename T, bool Small = (sizeof(T) == 1)>
struct is_small : false_type {};
template <typename T>
struct is_small<T, true> : true_type {};

static_assert(!is_small<int>::value, "***");
static_assert(is_small<char>::value, "char is supposed to be small");
//
// another implementation of issmall
//
template <bool Cond>
struct enable_if
{
    /* empty body*/
};
template <>
struct enable_if<true>
{
    typedef void type;
};
// a notational convenience
//template <bool Cond>
//using enable_if_t = typename enable_if<Cond>::type;

template <typename T, typename /*U*/ = void>
struct is_small2 : false_type {};

template<typename T>
struct is_small2<T, typename enable_if<sizeof(T) == 1>::type> :true_type{};

//
//template <typename T>
//struct is_small2B<T, enable_if_t<sizeof(T) == 1>> : true_type {};



}