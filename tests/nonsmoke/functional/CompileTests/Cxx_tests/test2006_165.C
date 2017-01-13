
class A
   {
     public:
       // Specification of a friend is sufficient to declare a scope for "void foo();", 
       // but only for pre g++ 4.1.0 versions.
       // friend void foo();
          void foo();
   };

void foobar()
   {
  // This will put a function symbol into the local scope
  // DQ (10/18/2006): This is fixed to not be added to the local sysmbol table.
  // Non-defining declarations in scopes where supportsFunctionDefinition() is 
  // false are not added to the local sysmbol table.
     void foo();

  // This function call requires the previous prototype, and can not use a global qualifier.
     foo();
   }

// This will put a function symbol into the global scope (because supportsFunctionDefinition()
// is true for SgGlobal (global scope).  In g++ versions before 4.1.0, a friend function
// declared in a class will also associate a global scope to "void foo();", version 4.1.0
// claims to more closely follow the standard on this detail, so I assume C++ is this way.
void foo();

void fubar()
   {
  // This function can use a qualified scope (it is optional, since there is no ambiguity).
     ::foo();
   }


// This will re-insert a function symbol into the global scope (redundantly, but OK)
void foo() {}

