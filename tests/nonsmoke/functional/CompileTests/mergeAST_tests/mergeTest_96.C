template<typename T, typename U>
class X
   {
     public:
       // virtual void do_toupper(const T & t) const = 0;
       // void local_foo(X __is){}

          template<typename T1, typename T2>
          void foo(X<T1,T2> __is){};
   };

// template<typename T1, typename T2> void X<int,int>::foo(X<T1,T2> __is){}

X<int,int> x;
X<int,int> y;

void foobar()
   {
     x.foo(y);
   }

