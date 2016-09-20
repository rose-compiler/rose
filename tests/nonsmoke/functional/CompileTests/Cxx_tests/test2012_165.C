// struct B {};

struct A
   {
  // Note that EDG automatically inserts a forward declaration for struct B (but this is wrong, since it declares struct  B to be in struct A).
  // 1) We don't want EDG to output this extra declaration.
  // struct B;

  // This is a declaration of a pointer to the global struct B (which is not declared in global scope).
  // 2) This reference to B should not be qualified since it's type has not been seen as a declaration.
     struct B *bp;

  // The declaration of "struct B;", after the declaration, changes the scope of the B used in the declaration 
  // of "struct B *bp;" so that B is in the namespace outside of the class (global scope in this case).
  // Now there is a vailid struct B in A, so A::B*y2 is vaild.
     struct B;

//   struct B {};
   };

// struct B {};
#if 0
int
main()
   {
  // This will cause an error, since "x.bp = y;" will not type match.
  // struct B {};

     A x;

  // Note that struct B has a non-defining declaration as a struct in global scope (though it was never declared in global scope).
     A::B* y;
     A::B* y2;

  // B z; // Of course this is an error, because the definition of b has not been seen.

  // so this is allowed (because bp is a pointer to the global B not the one in struct A.
     x.bp = y;

  // x.bp = y2; // This is an error since A::bp is a pointer to the global struct B not the local one in this function.
   }
#endif
