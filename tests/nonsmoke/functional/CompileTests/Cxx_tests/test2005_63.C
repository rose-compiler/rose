// Test code to test use of explicit template declarations (not sure what they mean but they cause 
// instantiations to be generated in g++).

// NOTE: Need to test variables and function declarations as well (anything else???)

// #include <string>
// using namespace std;
// Explicit instantiation of templated member function
// template basic_string<char> & basic_string<char>::append<string::iterator>(string::iterator __first, string::iterator __last);

template <typename T>
class X
   {
     public: 
          void foo(T t) {}
   };

template void X<int>::foo(int t);

// Use X::foo(int) so that we can force it to be instantiated as
// an alternative to explicit template instatiation directive.
void foobar()
   {
     X<int> x;
     x.foo(0);
   }

