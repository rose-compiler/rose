#if 0
// Robert Preissl, May 22 2007, BUG in ROSE: using declaration for enums

namespace B {
   enum E2 { en21, en22 };
}

int main() {
   using B::E2; // causes error :
/*
ERROR: Default reached in sage_gen_using_declaration(): symbol = SgEnumSymbol
HiddenList: ../../../../NEW_ROSE_MAY7/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:33608: SgUsingDeclarationStatement* sage_gen_using_declaration(a_using_decl*): Assertion `false' failed.
Aborted
*/
   return 0;

} 
#endif


namespace B
   {
     enum E2 { en21, en22 };
   }

int main()
   {
     using B::E2; // causes error :
     return 0;
   } 
