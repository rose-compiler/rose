#if 0
// namespace std {

     template<typename T> class X;
     template<typename T> void foobar(X<T> & in);

     template<typename T>
     class X
        {
          public:
               T x;

               template<typename T1> friend void foobar(X<T1>&);
        };

  // template<typename T> X<T>& operator>>(X<T> & in, char* s);
  // template<typename T> void foobar(X<T> & in);

  // Explicit specialization declaration, defined in src/istream.cc.
     template<> void foobar<char>(X<char> & in);
// }

void foo()
   {
     X<char> x;
     foobar<char>(x); 
   }
#endif

template<typename T> class A;
template<typename T>
class B {
  template<typename T2> friend void f(A<T2>);
};
extern template class B<char>;
template<> void f(A<char>);
