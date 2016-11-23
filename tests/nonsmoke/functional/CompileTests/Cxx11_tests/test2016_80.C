struct T1
   {
     int mem1;
     T1() { } // user-provided default constructor
   };

struct T2
{
    int mem1;
    T2() { } // user-provided default constructor
    T2(const T2&) { } // user-provided copy constructor
    T2(const T1&) { } 
};

struct T3
   {
     int mem1;
     T3() { } // user-provided default constructor
     T3(const T2&) { } // user-provided copy constructor
     T3(const T1&) { } 
   };

template <typename _Tp> void __helper(const _Tp&);

// template <typename _Tp> decltype(__helper<const _Tp&>({})) foo();
// template <typename _Tp> decltype(__helper<const _Tp&>({{}})) foo2();
// template <typename _Tp> decltype(__helper<const _Tp&>({{{}}})) foo3();
template <typename _Tp>  void foo1(decltype(__helper<const _Tp&>({})));
template <typename _Tp>  void foo2(decltype(__helper<const _Tp&>({{}})));
template <typename _Tp>  void foo3(decltype(__helper<const _Tp&>({{{}}})));

void foobar()
   {
     T1 t1;
     T2 t2;
     T3 t3;
     foo1<T1>(t1);
     foo2<T2>(t2);
     foo3<T3>(t3);
   }
