struct T1
   {
     int mem1;
   };

struct T2
   {
     int mem1;
   };

struct T3
   {
     int mem1;
   };

template <typename _Tp> void function(const _Tp&);

template <typename _Tp> decltype(function<const _Tp&>({}))*     foo1() { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{}}))*   foo2() { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{{}}}))* foo3() { return 0L; }

void foobar()
   {
     foo1<T1>();
     foo2<T2>();
     foo3<T3>();
   }
