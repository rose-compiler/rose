// This test code demonstrates the required specialization of "template <> int X<int>::foo()"
// This test is very similar to test2005_136.C except that the template member function is 
// defined outside of the class.

#define REQUIRED 1

// ****************************
// Member function form of test
// ****************************

// Class containing member function not availabe to primative types
class A
   {
     public:
          int increment() { return 0; }
   };

// Template class containing member function (defined outside the class)
template <typename T>
class X
   {
     private:
          T t;

     public:
          int foo();
   };

// Template defined outside of the templated class
// template function containing member function that would 
// be an error to instantiate with a primative type
template <typename T>
int X<T>::foo()
   {
  // This would be an error if T was a primative type
     return t.increment();
   }

#if REQUIRED
// specialization for X::foo() when X is instantiated with an "int" 
// (primative, non class, type).  Without this specialized template 
// function the template instatiation directive would generate an 
// error.

// Note that when the X<int> class is output, foo is a member function
// of an instantiated class and output of "template <>" must be supressed
// (yet another silly template detail).  It is also correct to not
// specify it.
// template <>

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ == 4)
template <>
#endif
int X<int>::foo()
   {
     int x;
     return x++;
   }
#endif

// The explicit declaration of a variable of type X<A> which forces X<A>
// to be instantiated does not interfere with the directive immediately 
// following this declaration to explicitly instantiate X<A>.
// X<A> a;

// Template Instantiation Directive
// This works because "A" has a member function "increment"

// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
// TOO (2/15/2011): error for Thrifty g++ 3.4.4
#if (__GNUC__ == 3 && __GNUC_MINOR__ != 4)
template X<A>;

// Template Instantiation Directive
// This would fail if the specialization is not output properly
template X<int>;
#endif


int main()
   {
     X<A> a;
     X<int> b;
     a.foo();
     b.foo();
   }


/*
   In the unparsed code the member function is output, 
since it was not transformed it is not required.  It is
also an error in this case because it appeared after the
instantiation in "int main()" though a function prototype
placed before "int main()" could fix that.

int X < A > ::foo()
{
  return (this) -> t.increment();
}
*/
