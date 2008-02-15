
#define DEMONSTRATE_BUG 1

class X
   {
     public:
          int foo() 
             {
            // Declare a local variable but return the class data member (just to test the name resolution!)
               int isparallel = X::isparallel;
#if DEMONSTRATE_BUG
            // Use of isparallel before declaration causes static modifier to be omitted
               return X::isparallel;
#else
               return 42;
#endif
             }

          static int isparallel;
   };

// unparses to be: "int isparallel = -1;" if referenced in class ahead 
// of its declaration (case: DEMONSTRATE_BUG == 1)
int X::isparallel = -1;

void foo()
   {
#if 1
     int a = X::isparallel;
     X::isparallel;
#endif
   }

