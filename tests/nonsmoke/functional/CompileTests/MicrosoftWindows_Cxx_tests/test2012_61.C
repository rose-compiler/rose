template<class T>
class DEF
   {
     public:
          T xyz;

          T foo ();
   };

// This template will generate a mapping from an EDG IR node in both the defining and non-defining template maps.
// this is because it is first entered into the non-defining map when the prototype is processed and then entered
// into defining map when "T DEF<T>::foo()" is processed.

#if 1
template<class T>
T DEF<T>::foo ()
   { 
     return xyz;
   }
#endif

#if 1
int main()
   {
     DEF<int> object2;
  // object2.xyz = 8;
  // object2.xyz = object2.foo();

     return 0;
   }
#endif
