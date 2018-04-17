struct T1
   {
     int mem1;
     T1() { }  // user-provided default constructor
   };

struct T2
   {
     int mem1;
     T2(const T1&) { } 
   };

struct T3
   {
     int mem1;
     T3(const T2&) { } // user-provided copy constructor
   };

template <typename _Tp> void function(const _Tp&);

template <typename _Tp> decltype(function<const _Tp&>({})) foo1();
template <typename _Tp> decltype(function<const _Tp&>({{}})) foo2();
template <typename _Tp> decltype(function<const _Tp&>({{{}}})) foo3();

void foobar()
   {
     foo1<T1>();
     foo2<T2>();
     foo3<T3>();
   }
