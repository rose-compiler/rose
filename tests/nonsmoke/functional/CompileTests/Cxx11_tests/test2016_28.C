// template<typename... Arguments> void SampleFunction(Arguments... parameters);
// template<> void SampleFunction<int, int>(16, 24);

template<class ... Types> struct Tuple {};
Tuple<> t0;           // Types contains no arguments
Tuple<int> t1;        // Types contains one argument: int
Tuple<int, float> t2; // Types contains two arguments: int and float
// Tuple<0> error;       // error: 0 is not a type

template<class ... Types> void f(Types ... args);

void foobar()
   {
     f();       // OK: args contains no arguments
     f(1);      // OK: args contains one argument: int
     f(2, 1.0); // OK: args contains two arguments: int and double
   }


#if 0
#include <iostream>
 
void tprintf(const char* format) // base function
{
    std::cout << format;
}
 
template<typename T, typename... Targs>
void tprintf(const char* format, T value, Targs... Fargs) // recursive variadic function
{
    for ( ; *format != '\0'; format++ ) {
        if ( *format == '%' ) {
           std::cout << value;
           tprintf(format+1, Fargs...); // recursive call
           return;
        }
        std::cout << *format;
    }
}

int main()
{
    tprintf("% world% %\n","Hello",'!',123);
    return 0;
}
#endif

#if 0
struct B { void g(); };
struct D { void g(); };

template <typename... bases>
struct X : bases... {
	using bases::g...;
};
X<B, D> x; // OK: B::g and D::g introduced
#endif


