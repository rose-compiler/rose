struct T1
   {
     int mem1;
   };

template <class _Tp> void function(const _Tp&);

template <class _Tp> decltype(function<const _Tp&>({}))*             foo1();
template <class _Tp> decltype(function<const _Tp&>({{}}))*           foo2();
template <class _Tp> decltype(function<const _Tp&>({{{}}}))*         foo3();
template <class _Tp> decltype(function<const _Tp&>({{{{}}}}))*       foo4();
template <class _Tp> decltype(function<const _Tp&>({{{{{}}}}}))*     foo5();
template <class _Tp> decltype(function<const _Tp&>({{{{{{}}}}}}))*   foo6();
template <class _Tp> decltype(function<const _Tp&>({{{{{{{}}}}}}}))* foo6();

void foobar()
   {
     foo1<T1>();
     foo2<T1>();
     foo3<T1>();
     foo4<T1>();
     foo5<T1>();
     foo6<T1>();
     foo7<T1>();
   }
