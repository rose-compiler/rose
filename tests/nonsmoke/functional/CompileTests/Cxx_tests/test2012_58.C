// This code is similar to test2012_57.C.
template <typename T>
class X
   {
     public:
#if 1
       // This function is not called so it will be listed internally in EDG as not "defined" 
       // and this causes some problems for the EDG/ROSE translation.
          friend void foo( X<T> & i )
             {
               int a;
             }
#endif
   };

#if 1
template <typename T>
void foo( T & j )
   {
     int b;
   }
#endif

#if 1
void foobar()
   {
     X<int> x;

  // This causes the EDG/ROSE connection to fail (this will cause the foo explicitly defined in the global scope to be referenced).
     ::foo(x);
   }
#endif
