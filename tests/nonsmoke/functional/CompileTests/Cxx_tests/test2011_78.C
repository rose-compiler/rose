// This is a slight variation on test2005_179.C.

class foo
   {
     public:
          void prod ();
   };

// Function appearing before declaration of prod in global scope
void foobar ()
   {
  // forward declaration does not imply that the "prod" function is in global scope (just that it exists).
  // This declaration does not associate a scope to "void prod()"
     void prod (int x);

  // Can't use global qualification here since there is no "prod" function declared in global scope.
  // Since we don't know exactly where "void prod()" is we can't use explicit qualification.
     prod (0);
   }

// This says that it exists and that it is declared in global scope (some sort of stronger forward declaration)
// This defines the scope of "void prod()"
// void prod (int x);

void foo::prod ()
   {
  // Enough to declare the function, but not strong enough to say what the qualified name is when it is called.
  // Again, this does not associate the scope with "void prod(int)"
     void prod (int x);

  // Error: global scope name qualification is dropped in generated code!
  // global qualification here is required!
#if 0
     ::prod (0);
#else
     prod (0);
#endif
   }

void prod (int x);
