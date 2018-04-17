// template<class T> class X { };
// template<class T> void f(X<T>);
// template<> void f(X<int>);

namespace std
   {
   }


namespace std
   {
     template<class T> class X { };
     template<class T> void f(X<T>) {}
     template<> void f(X<int>) {}
   }


# if 0
template < typename T >
void foobar (T & x)
   {
   }

// The explicit specialization of the template function without using a template class specialization does not appear to be possible.
template<> 
void foobar ( int x )  
   {
   }
#endif

