// t0500.cc
// template template parameters
// from gcc-3.4.3 <valarray> header

template < class _Oper, 
           template < class, class > class _Meta1,
           template < class, class > class _Meta2, 
           class _Dom1,
           class _Dom2 > 
class _BinClos;
