
// Since EDG normalizes the AST to move all member function definitions outside of 
// the class these separate clases are really the same case internally to ROSE.
#define DEFINE_LOCAL_TO_CLASS 0

class X
   {
     public:
#if DEFINE_LOCAL_TO_CLASS
          void foo (){};
#else
          void foo ();
#endif
   };

#if DEFINE_LOCAL_TO_CLASS
// It is a C++ error to redeclare member function once it is previously defined!
// void X::foo();
#else
void X::foo() {}
#endif
