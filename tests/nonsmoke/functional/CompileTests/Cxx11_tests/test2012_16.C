// Null pointer constant
// For the purposes of this section and this section alone, every occurrence of \u201c0\u201d 
// is meant as \u201ca constant expression which evaluates to 0, which is of type int\u201d. 
// In reality, the constant expression can be of any integral type.

using namespace std;

// Since the dawn of C in 1972, the constant 0 has had the double role of constant integer and 
// null pointer constant. The ambiguity inherent in the double meaning of 0 was dealt with in C 
// by the use of the preprocessor macro NULL, which commonly expands to either ((void*)0) or 0. 
// C++ didn't adopt the same behavior, allowing only 0 as a null pointer constant. This interacts 
// poorly with function overloading:

void foo(char *);
void foo(int);

// If NULL is defined as 0 (which is usually the case in C++), the statement foo(NULL); will call 
// foo(int), which is almost certainly not what the programmer intended, and not what a superficial 
// reading of the code suggests.

// C++11 corrects this by introducing a new keyword to serve as a distinguished null pointer 
// constant: nullptr. It is of type nullptr_t, which is implicitly convertible and comparable to 
// any pointer type or pointer-to-member type. It is not implicitly convertible or comparable to 
// integral types, except for bool. While the original proposal specified that an rvalue of type 
// nullptr should not be convertible to bool, the core language working group decided that such a 
// conversion would be desirable, for consistency with regular pointer types. The proposed wording 
// changes were unanimously voted into the Working Paper in June 2008.[2]

// For backwards compatibility reasons, 0 remains a valid null pointer constant.

char *pc = nullptr;     // OK
int  *pi = nullptr;     // OK

// C++11 web pages suggest that this should work, but it fails for GNU g++ version 6.1 C++11 support.
// bool   b = nullptr;     // OK. b is false.

// int    i = nullptr;     // error
 
void foobar2()
   {
     foo(nullptr);           // calls foo(char *), not foo(int);
   }

