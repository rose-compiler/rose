// This test code demonstrates the requirements for "::" (global qualification).

class myNumberType
   {
     public:
         void foobar();
   };

typedef ::myNumberType myNumberType;

typedef myNumberType numberType;

class Y
   {
     public:
          typedef int numberType;
   };

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

class Z : public Y
   {
     public:
       // Such typedefs are significant (but force 
       // "typedef ::numberType numberType;" to be an error)
       // typedef numberType numberType;

       // Example of where "::" is significant, without it numberType 
       // will be an "int" instead of a "myNumberType"
          typedef ::numberType numberType;

       // Note also that if Z were not derived from Y then 
          typedef numberType numberType;
       // could have been used to refer to myNumberType

          void foo()
             {
            // Unparsed as Z::numberType numberValue, which is a problem since the typedef base type is not unparsed as "::numberType"
               numberType numberValue;

           // function "foobar()" not defined for "int"
               numberValue.foobar();
             }

         ::numberType foo ( ::numberType numberValue );
#if 0
         ::numberType foo ( numberType numberValue, int x );

      // The code generation will output a redundant "::" (global qualifier) becuase EDG things it is required.
         ::numberType foo ( Y::numberType numberValue, int x );

      // The global qualifier for the function parameter is marked as required, but is not REALLY needed.
         ::numberType foo ( ::Y::numberType numberValue );
#endif
   };

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

