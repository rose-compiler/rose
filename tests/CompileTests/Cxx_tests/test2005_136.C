// This test code demonstrates the required specialization of "template <> int X<int>::foo()"

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

// template function containing member function that would 
// be an error to instantiate with a primative type
template <typename T>
class X
   {
     private:
          T t;

     public:
          int foo()
             {
            // This would be an error if T was a primative type
               return t.increment();
             }
   };

#if REQUIRED
// specialization for X::foo() when X is instantiated with an "int" 
// (primative, non class, type).  Without this specialized template 
// function the template instatiation directive would generate an error.
template <> int X<int>::foo()
   {
     int x;
     return x++;
   }
#endif

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

// Error: currently fails to generate code for prototype before use.
//        There is an instantiation of the class X<A> (specialization)
//        but we don't put that out since it is not required (not transformed)
//        and we let the vendor compiler instantiate the class where possible.
//        Thus we fail to output the declaration of the instantiation of the 
//        member functions of the class that we suppress.  In C++ if we supress
//        the class containing the member function's declaration we can still 
//        output the qualified member function as forward declaration.
//        so we have to output any required member function outside of the 
//        class.  To do this, copy the member function declaration in the class
//        and put it in the scope of the class declaration after the class.
//        do this only if the class is not output, if it is then we can't
//        output two member function declarations.
// int X < A > ::foo();


int main()
   {
     X<A> a;
     X<int> b;
     a.foo();
     b.foo();
   }


