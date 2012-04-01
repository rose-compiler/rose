template <typename T>
class X
   {
     public:
          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }
   };

template <typename T>
T & operator+( T & i, T & j)
   {
     return i;
   }

#if 0
void foobar()
   {
     X<int> x,y,z;

     x = y + z;
   }
#endif
