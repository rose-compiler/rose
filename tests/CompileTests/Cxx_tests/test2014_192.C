template <typename T>
class X
   {
     public:
#if 0
         static void free(X* ptr);
#else
         X() 
             {
               int s_value;
             }
#endif
   };

#if 0
template < typename T >
void X<T>::free ( X * ptr ) { delete ptr; }
#endif

void foo()
   {
      X<int> a;
   }

