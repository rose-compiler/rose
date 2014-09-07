template <typename T>
class X
   {
     public:
#if 1
      static void free(X* x);
#else
          static void free(X * ptr) 
             {
               delete x;
             }
#endif
   };

#if 1
template < typename T >
void X<T>::free ( X * ptr ) {}
#endif

void foo()
   {
      X<int>* a = 0L;
      X<int>::free(a);
   }

