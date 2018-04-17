// number #32

// class vector{};

class myVector
   {
     public:
       // class vector{};

       // This references a class that is not defined or declared elsewhere.
       // We build the declaration but we don't build a symbol (since we 
       // would not know what scope to associate it with).  By default we
       // define the scope of the SgClassDeclaration for class vector to be
       // SgGlobal. After checking it appears that EDG is assigning it to 
       // global scope (or it defaults to global scope, I think).
          myVector operator= (const class vector &x) const;
   };

// A test for empty intersection of symbols from all scopes would help
// avoid this sort of bug (but it would be expensive if not done well).

// If this declaration is present then a SgClassSymbol is generated
// in global scope AND it is referenced in the sysmbol table for the
// class definition for class myVector!  This is likely anohter bug!
// class vector{};
