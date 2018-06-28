namespace N {
struct A
   {
  // This is a declaration of a pointer to the global struct B (which is not declared in global scope).
  // BUG: this is unparse as: "struct N::B *bp;" instead of "struct B *bp;"
     struct B *bp;

     struct B {};
   };
}
