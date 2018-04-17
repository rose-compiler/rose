struct T1
   {
     int mem1;
   };

template <typename _Tp> void function(const _Tp&);

template <typename _Tp> decltype(function<const _Tp&>({}))*     foo1(); // { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{}}))*   foo2(); // { return 0L; }

// Note that GNU g++ version 6.1 can only handle 2 levels, while EDG 4.11 can handle 3 levels.
// template <typename _Tp> decltype(function<const _Tp&>({{{}}}))* foo3(); // { return 0L; }

#if 0
template <typename _Tp> decltype(function<const _Tp&>({{{{}}}}))*       foo4(); // { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{{{{}}}}}))*     foo5(); // { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{{{{{}}}}}}))*   foo6(); // { return 0L; }
template <typename _Tp> decltype(function<const _Tp&>({{{{{{{}}}}}}}))* foo7(); // { return 0L; }
#endif

void foobar()
   {
     foo1<T1>();
     foo2<T1>();
  // Note that GNU g++ version 6.1 can only handle 2 levels, while EDG 4.11 can handle 3 levels.
  // foo3<T1>();

#if 0
     foo4<T1>();
     foo5<T1>();
     foo6<T1>();
     foo7<T1>();
#endif
   }


