struct A
   {
  // Note that EDG automatically inserts a forward declaration for struct B (but this is wrong since it declares struct  B to be in struct A).
  // However, if we do it, then it means that "struct B;" is declared in "struct A" and so ROSE will generate the correct code.
     struct B;

  // This is a declaration of a pointer to the global struct B (which is not declared in global scope).
     struct B *bp;

  // Now there is a vailid struct B in A, so A::B*y2 is vaild.
  // struct B;

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
