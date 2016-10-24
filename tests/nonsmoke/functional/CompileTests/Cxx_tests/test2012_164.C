namespace N {
struct A
   {
  // Note that EDG altomatically inserts a forward declaration for struct B.
  // struct B;

  // This is a declaration of a pointer to the global struct B (which is not declared in global scope).
     struct B *bp;

     struct B {};
   };
}

// struct B {};

int
main()
   {
  // This will cause an error, since "x.bp = y;" will not type match.
  // struct B {};

      N::A x;

  // Note that struct B has a non-defining declaration as a struct in global scope (though it was never declared in global scope).
     N::B* y;
     N::A::B* y2;

  // B z; // Of course this is an error, because the definition of b has not been seen.

  // so this is allowed (because bp is a pointer to the global B not the one in struct A.
     x.bp = y;

  // x.bp = y2; // This is an error since A::bp is a pointer to the global struct B not the local one in this function.
   }
