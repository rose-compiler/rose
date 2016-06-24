// This test code demonstrates that the initialization of function parameters is not working
// "int y = foo();" works bug "void foobar(int i = foo());" is unparsed as "void foobar(int i);"

int foo();

class X
   {
     public:
          X(int i = foo());
   };

X::X(int i)
   {
   }

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

// Test use of default parameters in template member function declarations
template<typename T>
class Y
   {
     public:
          Y(T i = 1);
   };

template<typename T>
Y<T>::Y<T>( T i)
   {
   }

#if 0
// This is tested separately in test2005_87.C
// Test use of default parameters in template member function declarations
template<typename T>
class Z
   {
     public:
          Z(T i = 1) {};
   };

// Use of templated class using constructor specificed with default argument
Z<int> z;
#endif

void foo2(int i = foo());

// This demonstrase an error since the declaration of the 
// constructor for X has not defualt value because of this bug.
X x;

// This works fine!
int y = foo();

int a = 1;

void foobar(int i = 1);
// void foobar(int i = 1); // Error: can't redefine function parameter (even to the same value)

void foobar2(int i = 1)
{
}

void foobar3(int i);
void foobar3(int i = 1);

void foobar4(int i, int j);
void foobar4(int i, int j);
void foobar4(int i, int j = 1);

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

