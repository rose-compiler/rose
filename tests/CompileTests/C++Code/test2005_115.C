// This is an example from Peter, or code which GNU can not compile
// Such examples are interesting if only because it would be useful
// to verify that ROSE can compile them.

#include <stddef.h>

class A {};

template <class IprType> struct default_return_type
{ typedef void type;
};
  
template <>
struct default_return_type<A>
{ typedef int type;
};

template <template <class> class RetType>
struct Base
{
          typedef typename RetType<A>::type surrogate_t;
            
            template <class T>
                      void foo(T&, typename RetType<T>::type * ret) {}
                
              void foo1(A&, surrogate_t * ret) {}  
};

struct Concrete : Base<default_return_type>
{ void bar(A& a) { foo(a, NULL); }
          void bar0(A& a) { int * x = NULL; foo(a, x); }
            void bar1(A& a) { foo1(a, NULL); }
};
