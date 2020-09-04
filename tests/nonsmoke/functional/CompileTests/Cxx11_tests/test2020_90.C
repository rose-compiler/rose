// RC-106

struct X 
   {
     template<typename _Tp, typename Functor>
     void foo(const Functor & operation);
   };

template<typename _Tp, typename Functor>
void X::foo(const Functor & operation) 
   {
     operation(reinterpret_cast<int*>(0));
   }
