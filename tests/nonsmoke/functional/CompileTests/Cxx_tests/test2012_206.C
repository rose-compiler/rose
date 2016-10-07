
template <class T> 
struct X
   {
     void foo(T* t);
     // { t->foobar(); }
   };

template <class T> 
void X<T>::foo(T* t)
   {
     t->foobar();
   }
