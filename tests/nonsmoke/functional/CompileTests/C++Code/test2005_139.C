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

template<>
class X<int>
   {
     private:
          int t;
     public:
          int foo() { return 0; }
   };

#endif

// Template Instantiation Directive
// This works because "A" has a member function "increment"
template X<A>;

// Template Instantiation Directive
// This would fail if the specialization X<int> is present
// template X<int>;

int main()
   {
     X<A> a;
     X<int> b;
     a.foo();
     b.foo();
   }

