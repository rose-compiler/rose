namespace N {
struct A
   {
  // This is a declaration of a pointer to the global struct B (which is not declared in global scope).
  // BUG: this is unparse as: "struct N::B *bp;" instead of "struct B *bp;"
     struct B *bp;

     struct B {};
   };
}

int
main()
   {
     struct N::A x;

  // Note that struct B has a non-defining declaration as a struct in global scope (though it was never declared in global scope).
  // N::C* cy;
     struct N::B* by;

  // so this is allowed (because bp is a pointer to the global B not the one in struct A).
     x.bp = by;
   }
