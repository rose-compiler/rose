template <typename T>
class X
   {
     public:
#if 0
         static void free(X* ptr);
#else
         template <typename S>
         void free(X * ptr) 
             {
               S s_value;
               delete ptr;
             }
#endif
   };

#if 0
template < typename T >
void X<T>::free ( X * ptr ) { delete ptr; }
#endif

void foo()
   {
      X<int>* a = 0L;
      a->free<long>(a);
   }

