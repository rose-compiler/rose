// Note that if we set #define SKIP_ROSE_BUILTIN_DECLARATIONS
// in the rose_edg_required_macros_and_function.h file then 
// this causes an error for this code.  I don't know why!
// This value is only set for debugging, and so it is not
// a problem in reality, but it is curious and should be
// looked into at some point.

class XXX
   {
     public:
      // BAD IDEA: This typedef's base type needs to reference the non-defining, but it needs to be a seperate one 
      // from that used for the symbol (so that the parent can be set properly, or we need to only ask 
      // for parent from the defining declaration).
      // INSTEAD: If the non-defining declaration is not referenced in the AST then it could be anything 
      // (perhaps matching the defining declaration, if it exists, and NULL if not).  If the non-defining 
      // declaration IS referenced in the AST, then it has to have a parent that structurally matches
      // it's reference in the AST.  If there are multiple refeences from the AST, then the non-defining 
      // declaration must be replicated (to maintaine unique statements in the AST).
      // TESTS: The AST Consistancy Tests must check this policy.
         typedef XXX *XXXstar;

      // XXX (int) {}
      // int x;
      // void foo() {}
      // void foo(int,double,float,double*,float*,long,int,double,float,double*,float*,long);
         void foo() {}
   };

