#define DEMONSTRATE_BUG 1

class X
   {
     public:
          void foo() 
             {
            // This causes the symbol generated in class X to reference the 
            // SgInitializedName built to hold the reference from the SgVarRefExp.
#if DEMONSTRATE_BUG
               int i = X::isparallel;
#endif
             }
          static int isparallel;
   };

int X::isparallel = -1;

void foobar()
   {
#if 1
  // int a = X::isparallel;
     X::isparallel;
#endif
   }
