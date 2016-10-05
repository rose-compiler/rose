// test2007_51.C

#if 0
The following code (extracted from BusyBox):

void f() {
 ({ union { int i; } x; });
}

fails to unparse with the following error:

identityTranslator: /home/willcock2/ROSE/src/backend/unparser/unparse_stmt.C:2867: void Unparser::unparseVarDeclStmt(SgStatement*, SgUnparse_Info&): Assertion `ninfo.SkipClassDefinition() == false' failed.

This probably isn't a high priority bug because it relates to a GNU extension, but BusyBox might be relevant to analyzing code for some embedded devices.

-- Jeremiah Willcock 

#endif

void f()
   {
  // union { int i; } x;      // works
  // { union { int i; } x; }; // works
  // ({ union { int i; } x; });
  // ({ struct { int i; } x; }); // fails
  // { struct { int i; } x; }
     ({ struct { int i; } x; });
   }
