// This is a subset of test2005_179.C

// Function appearing before declaration of prod in global scope
void foobar ()
   {
  // forward declaration does not imply that the "prod" function is in global scope (just that it exists).
  // This declaration does not associate a scope to "void prod()"
     void prod (int x);

  // Can't use global qualification here since there is no "prod" function declared in global scope.
  // Since we don't know exactly where "void prod()" is we can't use explicit qualifiaction.
     prod (0);
   }
