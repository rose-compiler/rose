// #include "stdio.h"

template<class T>
class DEF
   {
     public:
          T xyz;
          T foo ();
   };

template<class T>
T DEF<T>::foo ()
   { 
  // printf ("Inside of first template code processed using ROSE! \n"); 
     return xyz;
   }

// DQ (9/20/2013): It is an error to unparse the template 
// instantiation that is generated internally.

int main()
   {
     DEF<int> object2;
     object2.xyz = 8;
     object2.xyz = object2.foo();

     return 0;
   }
