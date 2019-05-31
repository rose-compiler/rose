

template < class T>
void foobar();

namespace X
   {
     class A {};
   }

void foo()
   {
     foobar<X::A>();
   }

