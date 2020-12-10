#include <string>
#include <array>
struct S {
    int x;
    struct Foo {
        int i;
        int j;
        int a[3];
    } b;
};
 
union U {
    int a;
    const char* b;
};
 
int main()
{
    S s1 = { 1, { 2, 3, {4, 5, 6} } };
    S s2 = { 1, 2, 3, 4, 5, 6}; // same, but with brace elision
    S s3{1, {2, 3, {4, 5, 6} } }; // same, using direct-list-initialization syntax
    S s4{1, 2, 3, 4, 5, 6}; // error in C++11: brace-elision only allowed with equals sign
                            // okay in C++14
 
    int ar[] = {1,2,3}; // ar is int[3]
    int ab[] (1, 2, 3); // (C++20) ab is int[3] 
//  char cr[3] = {'a', 'b', 'c', 'd'}; // too many initializer clauses
    char cr[3] = {'a'}; // array initialized as {'a', '\0', '\0'}
 
    int ar2d1[2][2] = {{1, 2}, {3, 4}}; // fully-braced 2D array: {1, 2}
                                        //                        {3, 4}
    int ar2d2[2][2] = {1, 2, 3, 4}; // brace elision: {1, 2}
                                    //                {3, 4}
    int ar2d3[2][2] = {{1}, {2}};   // only first column: {1, 0}
                                    //                    {2, 0}
 
    std::array<int, 3> std_ar2{ {1,2,3} };    // std::array is an aggregate
    std::array<int, 3> std_ar1 = {1, 2, 3}; // brace-elision okay
 
    int ai[] = { 1, 2.0 }; // narrowing conversion from double to int:
                           // error in C++11, okay in C++03
 
    std::string ars[] = {std::string("one"), // copy-initialization
                         "two",              // conversion, then copy-initialization
                         {'t', 'h', 'r', 'e', 'e'} }; // list-initialization
 
    U u1 = {1}; // OK, first member of the union
//    U u2 = { 0, "asdf" }; // error: too many initializers for union
//    U u3 = { "asdf" }; // error: invalid conversion to int
 
}
 
// aggregate
struct base1 { int b1, b2 = 42; };
// non-aggregate
struct base2 {
  base2() : b3(42) {}
  int b3;
};
// aggregate in C++17
struct derived : base1, base2 { int d; };
derived d1{ {1, 2}, { }, 4}; // d1.b1 = 1, d1.b2 = 2,  d1.b3 = 42, d1.d = 4
derived d2{ {    }, { }, 4}; // d2.b1 = 0, d2.b2 = 42, d2.b3 = 42, d2.d = 4

