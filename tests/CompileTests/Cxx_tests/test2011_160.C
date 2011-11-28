// #include "stdio.h"

template<class T>
class DEF
   {
     public:
          T xyz;
          T foo ();
   };

#if 0
template<class T>
T DEF<T>::foo ()
   { 
  // printf ("Inside of first template code processed using ROSE! \n"); 
     return xyz;
   }
#endif

#if 0
int main()
   {
     DEF<int> object2;
     object2.xyz = 8;
     object2.xyz = object2.foo();

     return 0;
   }
#endif
